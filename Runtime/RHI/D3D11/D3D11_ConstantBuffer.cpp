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

//= INCLUDES =====================
#include "../RHI_ConstantBuffer.h"
#include <d3d11.h>
#include "../../Logging/Log.h"
#include "../RHI_Device.h"
//================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Directus
{
	RHI_ConstantBuffer::RHI_ConstantBuffer(shared_ptr<RHI_Device> rhiDevice)
	{
		m_rhiDevice = rhiDevice;
		m_buffer	= nullptr;
	}

	RHI_ConstantBuffer::~RHI_ConstantBuffer()
	{
		if (m_buffer)
		{
			((ID3D11Buffer*)m_buffer)->Release();
			m_buffer = nullptr;
		}
	}

	bool RHI_ConstantBuffer::Create(unsigned int size)
	{
		if (!m_rhiDevice || !m_rhiDevice->GetDevice<ID3D11Device>())
		{
			LOG_ERROR("RHI_ConstantBuffer::Create: Invalid RHI device");
			return false;
		}

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth			= size;
		bufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags			= 0;
		bufferDesc.StructureByteStride	= 0;

		HRESULT result = m_rhiDevice->GetDevice<ID3D11Device>()->CreateBuffer(&bufferDesc, nullptr, (ID3D11Buffer**)&m_buffer);
		if FAILED(result)
		{
			LOG_ERROR("RHI_ConstantBuffer::Create: Failed to create constant buffer");
			return false;
		}

		return true;
	}

	void* RHI_ConstantBuffer::Map()
	{
		if (!m_rhiDevice || !m_rhiDevice->GetDeviceContext<ID3D11DeviceContext>())
		{
			LOG_ERROR("RHI_ConstantBuffer::Map: Invalid RHI device");
			return nullptr;
		}

		if (!m_buffer)
		{
			LOG_ERROR("RHI_ConstantBuffer::Map: Invalid buffer");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = m_rhiDevice->GetDeviceContext<ID3D11DeviceContext>()->Map((ID3D11Buffer*)m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			LOG_ERROR("RHI_ConstantBuffer::Map: Failed to map constant buffer.");
			return nullptr;
		}

		return mappedResource.pData;
	}

	bool RHI_ConstantBuffer::Unmap()
	{
		if (!m_rhiDevice || !m_rhiDevice->GetDeviceContext<ID3D11DeviceContext>())
		{
			LOG_ERROR("RHI_ConstantBuffer::Unmap: Invalid RHI device");
			return false;
		}

		if (!m_buffer)
		{
			LOG_ERROR("RHI_ConstantBuffer::Unmap: Invalid buffer");
			return false;
		}

		m_rhiDevice->GetDeviceContext<ID3D11DeviceContext>()->Unmap((ID3D11Buffer*)m_buffer, 0);

		return true;
	}
}