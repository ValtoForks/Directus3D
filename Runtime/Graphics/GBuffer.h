/*
Copyright(c) 2016-2017 Panos Karabelas

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

//= INCLUDES =====================================
#include <vector>
#include "../Math/Vector4.h"
#include "../Graphics/D3D11/D3D11GraphicsDevice.h"
//================================================

namespace Directus
{
	struct GBufferTex
	{
		DXGI_FORMAT format;
		ID3D11Texture2D* renderTexture;
		ID3D11RenderTargetView* renderTargetView;
		ID3D11ShaderResourceView* shaderResourceView;
	};

	class GBuffer
	{
	public:
		GBuffer(D3D11GraphicsDevice* graphicsDevice);
		~GBuffer();

		bool Create(int width, int height);
		bool SetAsRenderTarget();

		bool Clear(const Directus::Math::Vector4& color);
		bool Clear(float red, float green, float blue, float alpha);

		ID3D11ShaderResourceView* GetShaderResourceView(int index);

	private:
		std::vector<GBufferTex> m_renderTargets;

		// Depth buffer
		ID3D11Texture2D* m_depthStencilBuffer;
		ID3D11DepthStencilView* m_depthStencilView;
		float m_maxDepth = 1.0f;

		// Dimensions
		D3D11_VIEWPORT m_viewport;
		int m_width, m_height;

		// Dependencies
		D3D11GraphicsDevice* m_graphics;
	};
}