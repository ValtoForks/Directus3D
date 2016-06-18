/*
Copyright(c) 2016 Panos Karabelas

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
#include "D3D11Buffer.h"
#include "D3D11Device.h"
#include "../../Misc/Globals.h"
#include "../../IO/Log.h"
//=============================

D3D11Buffer::D3D11Buffer()
{
	m_D3D11Device = nullptr;
	m_buffer = nullptr;
	m_stride = 0;
	m_size = 0;
	m_usage = (D3D11_USAGE)0;
	m_bindFlag = (D3D11_BIND_FLAG)0;
	m_cpuAccessFlag = (D3D11_CPU_ACCESS_FLAG)0;
}

D3D11Buffer::~D3D11Buffer()
{
	DirectusSafeRelease(m_buffer);
}

bool D3D11Buffer::Initialize(unsigned int size, D3D11Device* d3d11device)
{
	m_D3D11Device = d3d11device;
	m_size = size;
	m_usage = D3D11_USAGE_DYNAMIC;
	m_bindFlag = D3D11_BIND_CONSTANT_BUFFER;
	m_cpuAccessFlag = D3D11_CPU_ACCESS_WRITE;

	bool result = CreateBuffer(size, nullptr, m_usage, m_bindFlag, m_cpuAccessFlag);

	if (!result)
		LOG("Failed to create constant buffer.", Log::Error);

	return result;
}

bool D3D11Buffer::Initialize(std::vector<VertexPositionTextureNormalTangent> vertices, D3D11Device* d3d11device)
{
	m_D3D11Device = d3d11device;

	// convert
	VertexPositionTextureNormalTangent* vertexArray = new VertexPositionTextureNormalTangent[vertices.size()];
	for (unsigned int i = 0; i < vertices.size(); i++)
		vertexArray[i] = vertices[i];

	m_stride = sizeof(VertexPositionTextureNormalTangent);
	m_size = m_stride * vertices.size();
	m_usage = D3D11_USAGE_DEFAULT;
	m_bindFlag = D3D11_BIND_VERTEX_BUFFER;
	m_cpuAccessFlag = static_cast<D3D11_CPU_ACCESS_FLAG>(0);

	bool result = CreateBuffer(m_size, vertexArray, m_usage, m_bindFlag, m_cpuAccessFlag);

	if (!result)
		LOG("Failed to create vertex buffer.", Log::Error);

	delete[] vertexArray;

	return result;
}

bool D3D11Buffer::Initialize(std::vector<unsigned int> indices, D3D11Device* d3d11device)
{
	m_D3D11Device = d3d11device;

	// convert
	unsigned int* indexArray = new unsigned int[indices.size()];
	for (unsigned int i = 0; i < indices.size(); i++)
		indexArray[i] = indices[i];

	m_stride = sizeof(unsigned int);
	m_size = m_stride * indices.size();
	m_usage = D3D11_USAGE_DEFAULT;
	m_bindFlag = D3D11_BIND_INDEX_BUFFER;
	m_cpuAccessFlag = static_cast<D3D11_CPU_ACCESS_FLAG>(0);

	bool result = CreateBuffer(m_size, indexArray, m_usage, m_bindFlag, m_cpuAccessFlag);

	if (!result)
		LOG("Failed to create index buffer.", Log::Error);

	delete[] indexArray;

	return result;
}

void D3D11Buffer::SetIA()
{
	unsigned int offset = 0;

	if (m_bindFlag == D3D11_BIND_VERTEX_BUFFER)
		m_D3D11Device->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_buffer, &m_stride, &offset);
	else if (m_bindFlag == D3D11_BIND_INDEX_BUFFER)
		m_D3D11Device->GetDeviceContext()->IASetIndexBuffer(m_buffer, DXGI_FORMAT_R32_UINT, 0);
}

void D3D11Buffer::SetVS(unsigned int startSlot)
{
	m_D3D11Device->GetDeviceContext()->VSSetConstantBuffers(startSlot, 1, &m_buffer);
}

void D3D11Buffer::SetPS(unsigned int startSlot)
{
	m_D3D11Device->GetDeviceContext()->PSSetConstantBuffers(startSlot, 1, &m_buffer);
}

void* D3D11Buffer::Map()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = m_D3D11Device->GetDeviceContext()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		LOG("Failed to map buffer.", Log::Error);
		return nullptr;
	}

	return mappedResource.pData;
}

void D3D11Buffer::Unmap()
{
	m_D3D11Device->GetDeviceContext()->Unmap(m_buffer, 0);
}

/*------------------------------------------------------------------------------
									[PRIVATE]
------------------------------------------------------------------------------*/
bool D3D11Buffer::CreateBuffer(unsigned int size, void* data, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag, D3D11_CPU_ACCESS_FLAG cpuAccessFlag)
{
	// fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = usage;
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = bindFlag;
	bufferDesc.CPUAccessFlags = cpuAccessFlag;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// fill in the subresource data.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT result;
	if (bindFlag == D3D11_BIND_VERTEX_BUFFER || bindFlag == D3D11_BIND_INDEX_BUFFER)
		result = m_D3D11Device->GetDevice()->CreateBuffer(&bufferDesc, &initData, &m_buffer);
	else
		result = m_D3D11Device->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_buffer);

	if (FAILED(result))
		return false;

	return true;
}