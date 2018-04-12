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

#pragma once

//= INCLUDES ===================
#include "D3D11GraphicsDevice.h"
#include "../Vertex.h"
#include <vector>
//==============================

namespace Directus
{
	class D3D11VertexBuffer
	{
	public:
		D3D11VertexBuffer(D3D11GraphicsDevice* graphicsDevice);
		~D3D11VertexBuffer();

		bool Create(const std::vector<VertexPosCol>& vertices);
		bool Create(const std::vector<VertexPosTex>& vertices);
		bool Create(const std::vector<VertexPosTexTBN>& vertices);
		bool CreateDynamic(unsigned int stride, unsigned int initialSize);

		void* Map();
		bool Unmap();

		bool SetIA();

		unsigned int GetMemoryUsage() { return m_memoryUsage; }

	private:
		D3D11GraphicsDevice* m_graphics;
		ID3D11Buffer* m_buffer;
		unsigned int m_stride;
		unsigned int m_memoryUsage;
	};
}
