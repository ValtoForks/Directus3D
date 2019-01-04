/*
Copyright(c) 2016-2018 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define FREEIMAGE_LIB

//= INCLUDES =========================
#include "ImageImporter.h"
#include <FreeImage.h>
#include <Utilities.h>
#include "../../Threading/Threading.h"
#include "../../Core/Settings.h"
#include "../../RHI/RHI_Texture.h"
#include "../../Math/MathHelper.h"
//====================================

//= NAMESPACES =====
using namespace std;
//==================

namespace _ImagImporter
{
	FREE_IMAGE_FILTER rescaleFilter = FILTER_LANCZOS3;

	// A struct that rescaling threads will work with
	struct RescaleJob
	{
		unsigned int width		= 0;
		unsigned int height		= 0;
		unsigned int channels	= 0;
		vector<byte>* data		= nullptr;
		bool done				= false;

		RescaleJob(unsigned int width, unsigned int height, unsigned int channels)
		{
			this->width		= width;
			this->height	= height;
			this->channels	= channels;
		}
	};
}

namespace Directus
{
	ImageImporter::ImageImporter(Context* context)
	{
		m_context = context;
		FreeImage_Initialise(true);

		// Get version
		Settings::Get().m_versionFreeImage = FreeImage_GetVersion();
	}

	ImageImporter::~ImageImporter()
	{
		FreeImage_DeInitialise();
	}

	bool ImageImporter::Load(const string& filePath, RHI_Texture* texture)
	{
		if (!texture)
			return false;

		if (!FileSystem::FileExists(filePath))
		{
			LOGF_ERROR("ImageImporter::Load: Cant' load image. File path \"%s\" is invalid.", filePath.c_str());
			return false;
		}

		// Get image format
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filePath.c_str(), 0);

		// If the format is unknown
		if (format == FIF_UNKNOWN)
		{
			// Try getting the format from the file extension
			LOGF_WARNING("ImageImporter::Load: Failed to determine image format for \"%s\", attempting to detect it from the file's extension...", filePath.c_str());
			format = FreeImage_GetFIFFromFilename(filePath.c_str());

			// If the format is still unknown, give up
			if (!FreeImage_FIFSupportsReading(format))
			{
				LOGF_ERROR("ImageImporter::Load: Failed to detect the image format.");
				return false;
			}

			LOG_INFO("ImageImporter::Load: The image format has been detected succesfully.");
		}

		// Load the image
		FIBITMAP* bitmap = FreeImage_Load(format, filePath.c_str());
	
		// Perform some fix ups
		bitmap = ApplyBitmapCorrections(bitmap);

		// Perform any scaling (if necessary)
		bool userDefineDimensions	= (texture->GetWidth() != 0 && texture->GetHeight() != 0);
		bool dimensionMismatch		= (FreeImage_GetWidth(bitmap) != texture->GetWidth() && FreeImage_GetHeight(bitmap) != texture->GetHeight());
		bool scale					= userDefineDimensions && dimensionMismatch;
		bitmap						= scale ? _FreeImage_Rescale(bitmap, texture->GetWidth(), texture->GetHeight()) : bitmap;

		// Deduce image properties	
		bool image_transparency				= FreeImage_IsTransparent(bitmap);
		unsigned int image_width			= FreeImage_GetWidth(bitmap);
		unsigned int image_height			= FreeImage_GetHeight(bitmap);
		unsigned int image_bpp				= FreeImage_GetBPP(bitmap);
		unsigned int image_bpc				= ComputeBytesPerChannel(bitmap);
		unsigned int image_channels			= ComputeChannelCount(bitmap);
		Texture_Format image_format			= ComputeTextureFormat(image_bpp, image_channels);
		bool image_grayscale				= IsVisuallyGrayscale(bitmap);

		// Fill RGBA vector with the data from the FIBITMAP
		auto mip = texture->Data_AddMipLevel();
		GetBitsFromFIBITMAP(mip, bitmap, image_width, image_height, image_channels);

		// If the texture requires mip-maps, generate them
		if (texture->GetNeedsMipChain())
		{
			GenerateMipmaps(bitmap, texture, image_width, image_height, image_channels);
		}

		// Free memory 
		FreeImage_Unload(bitmap);

		// Fill RHI_Texture with image properties
		texture->SetBPP(image_bpp);
		texture->SetBPC(image_bpc);
		texture->SetWidth(image_width);
		texture->SetHeight(image_height);
		texture->SetChannels(image_channels);
		texture->SetTransparency(image_transparency);
		texture->SetFormat(image_format);
		texture->SetGrayscale(image_grayscale);

		return true;
	}

	bool ImageImporter::GetBitsFromFIBITMAP(vector<byte>* data, FIBITMAP* bitmap, unsigned int width, unsigned int height, unsigned int channels)
	{
		if (!data || width == 0 || height == 0 || channels == 0)
		{
			LOG_ERROR("ImageImporter::GetBitsFromFIBITMAP: Invalid parameters");
			return false;
		}

		// Compute expected data size and reserve enough memory
		unsigned int size = width * height * channels *  ComputeBytesPerChannel(bitmap);
		if (size != data->size())
		{
			data->clear();
			data->reserve(size);
			data->resize(size);
		}

		// Copy the data over to our vector
		auto bits = FreeImage_GetBits(bitmap);
		memcpy(&(*data)[0], bits, size);

		return true;
	}

	void ImageImporter::GenerateMipmaps(FIBITMAP* bitmap, RHI_Texture* texture, unsigned int width, unsigned int height, unsigned int channels)
	{
		if (!texture)
			return;
	
		// Create a RescaleJob for every mip that we need
		vector<_ImagImporter::RescaleJob> jobs;
		while (width > 1 && height > 1)
		{
			width	= Math::Helper::Max(width / 2, (unsigned int)1);
			height	= Math::Helper::Max(height / 2, (unsigned int)1);
			jobs.emplace_back(width, height, channels);
			
			// Resize the RHI_Texture vector accordingly
			unsigned int size = width * height * channels;
			vector<byte>* mip = texture->Data_AddMipLevel();
			mip->reserve(size);
			mip->resize(size);
		}

		// Pass data pointers (now that the RHI_Texture mip vector has been constructed)
		for (unsigned int i = 0; i < jobs.size(); i++)
		{
			// reminder: i + 1 because the 0 mip is the default image size
			jobs[i].data = texture->Data_GetMipLevel(i + 1);
		}

		// Parallelize mipmap generation using multiple threads (because FreeImage_Rescale() using FILTER_LANCZOS3 is expensive)
		auto threading = m_context->GetSubsystem<Threading>();
		for (auto& job : jobs)
		{
			threading->AddTask([this, &job, &bitmap]()
			{
				FIBITMAP* bitmapScaled = FreeImage_Rescale(bitmap, job.width, job.height, _ImagImporter::rescaleFilter);
				if (!GetBitsFromFIBITMAP(job.data, bitmapScaled, job.width, job.height, job.channels))
				{
					LOGF_ERROR("ImageImporter:GenerateMipmapsFromFIBITMAP: Failed to create mip level %dx%d", job.width, job.height);
				}
				FreeImage_Unload(bitmapScaled);
				job.done = true;
			});
		}

		// Wait until all mipmaps have been generated
		bool ready = false;
		while (!ready)
		{
			ready = true;
			for (const auto& job : jobs)
			{
				if (!job.done)
				{
					ready = false;
				}
			}
		}
	}

	unsigned int ImageImporter::ComputeChannelCount(FIBITMAP* bitmap)
	{	
		if (!bitmap)
		{
			LOGF_ERROR("ImageImporter::ComputeChannelCount: Invalid parameter");
			return 0;
		}

		// Compute the number of bytes per pixel
		unsigned int bytespp = FreeImage_GetLine(bitmap) / FreeImage_GetWidth(bitmap);

		// Compute the number of samples per pixel
		unsigned int channels = bytespp / ComputeBytesPerChannel(bitmap);

		return channels;
	}

	unsigned int ImageImporter::ComputeBytesPerChannel(FIBITMAP* bitmap)
	{
		FREE_IMAGE_TYPE type	= FreeImage_GetImageType(bitmap);
		unsigned int size		= 0;

		if (type == FIT_BITMAP)
		{
			size = sizeof(BYTE);
		}
		else if (type == FIT_UINT16 || type == FIT_RGB16 || type == FIT_RGBA16)
		{
			size = sizeof(WORD);
		}
		else if (type == FIT_FLOAT || type == FIT_RGBF || type == FIT_RGBAF)
		{
			size = sizeof(float);
		}

		return size;
	}

	Texture_Format ImageImporter::ComputeTextureFormat(unsigned int bpp, unsigned int channels)
	{
		if (channels == 3)
		{
			if (bpp == 96)
				return Texture_Format_R32G32B32_FLOAT;
		}
		else if (channels == 4)
		{
			if (bpp == 32)
				return Texture_Format_R8G8B8A8_UNORM;
		}
		
		LOG_ERROR("ImageImporter::ComputeTextureFormat: Failed to deduce channel count");
		return Texture_Format_R8G8B8A8_UNORM;
	}

	bool ImageImporter::IsVisuallyGrayscale(FIBITMAP* bitmap)
	{
		switch (FreeImage_GetBPP(bitmap))
		{
			case 1:
			case 4:
			case 8: 
			{
				unsigned ncolors = FreeImage_GetColorsUsed(bitmap);
				RGBQUAD *rgb = FreeImage_GetPalette(bitmap);
				for (unsigned i = 0; i < ncolors; i++) 
				{
					if ((rgb->rgbRed != rgb->rgbGreen) || (rgb->rgbRed != rgb->rgbBlue)) 
					{
						return false;
					}
				}
				return true;
			}
			default: 
			{
				return (FreeImage_GetColorType(bitmap) == FIC_MINISBLACK);
			}
		}
	}

	FIBITMAP* ImageImporter::ApplyBitmapCorrections(FIBITMAP* bitmap)
	{
		if (!bitmap)
		{
			LOGF_ERROR("ImageImporter::ApplyBitmapCorrections: Invalid parameter");
			return nullptr;
		}

		// Convert it to 32 bits (if lower)
		if (FreeImage_GetBPP(bitmap) < 32)
		{
			bitmap = _FreeImage_ConvertTo32Bits(bitmap);
		}

		// Swap red with blue channel (if needed)
		if (FreeImage_GetBPP(bitmap) == 32)
		{		
			if (FreeImage_GetRedMask(bitmap) == 0xff0000)
			{
				bool swapped = SwapRedBlue32(bitmap);
				if (!swapped)
				{
					LOG_WARNING("ImageImporter::ApplyBitmapCorrections: Failed to swap red with blue channel");
				}
			}
		}
		
		// Flip it vertically
		FreeImage_FlipVertical(bitmap);

		return bitmap;
	}

	FIBITMAP* ImageImporter::_FreeImage_ConvertTo32Bits(FIBITMAP* bitmap)
	{
		FIBITMAP* previousBitmap = bitmap;
		bitmap = FreeImage_ConvertTo32Bits(previousBitmap);
		FreeImage_Unload(previousBitmap);

		return bitmap;
	}

	FIBITMAP* ImageImporter::_FreeImage_Rescale(FIBITMAP* bitmap, unsigned int width, unsigned int height)
	{
		FIBITMAP* previousBitmap = bitmap;
		bitmap = FreeImage_Rescale(previousBitmap, width, height, _ImagImporter::rescaleFilter);
		FreeImage_Unload(previousBitmap);

		return bitmap;
	}
}
