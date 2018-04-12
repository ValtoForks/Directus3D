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

//= INCLUDES ==================
#include "D3D11Sampler.h"
#include "D3D11GraphicsDevice.h"
#include "../../Core/EngineDefs.h"
#include "../../Core/Settings.h"
#include "../../Logging/Log.h"
//=============================

namespace Directus
{
	D3D11Sampler::D3D11Sampler(D3D11GraphicsDevice* graphics)
	{
		m_graphics = graphics;
		m_sampler = nullptr;
	}

	D3D11Sampler::~D3D11Sampler()
	{
		SafeRelease(m_sampler);
	}

	bool D3D11Sampler::Create(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode, D3D11_COMPARISON_FUNC comparisonFunction)
	{
		if (!m_graphics->GetDevice())
			return false;

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = filter;
		samplerDesc.AddressU = textureAddressMode;
		samplerDesc.AddressV = textureAddressMode;
		samplerDesc.AddressW = textureAddressMode;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = ANISOTROPY_LEVEL;
		samplerDesc.ComparisonFunc = comparisonFunction;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = FLT_MIN;
		samplerDesc.MaxLOD = FLT_MAX;

		// create sampler state.
		HRESULT result = m_graphics->GetDevice()->CreateSamplerState(&samplerDesc, &m_sampler);
		if (FAILED(result))
		{
			LOG_INFO("Failed to create sampler.");
			return false;
		}

		return true;
	}

	bool D3D11Sampler::Set(unsigned int startSlot)
	{
		if (!m_graphics->GetDeviceContext())
			return false;

		m_graphics->GetDeviceContext()->PSSetSamplers(startSlot, 1, &m_sampler);

		return true;
	}
}