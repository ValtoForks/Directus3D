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

//= INCLUDES ==============================
#include "../../Math/Matrix.h"
#include "../../Math/Vector4.h"
#include "../D3D11/D3D11GraphicsDevice.h"
#include "../D3D11/D3D11ConstantBuffer.h"
#include "../D3D11/D3D11Shader.h"
#include "../../Scene/Components/Camera.h"
#include "../../Scene/Components/Light.h"
#include "../../Resource/ResourceManager.h"
//=========================================

namespace Directus
{
	class DeferredShader
	{
	public:
		DeferredShader();
		~DeferredShader();

		void Load(const std::string& filePath, Graphics* graphics);
		void UpdateMatrixBuffer(const Math::Matrix& mWorld, const Math::Matrix& mView, const Math::Matrix& mBaseView,
			const Math::Matrix& mPerspectiveProjection, const Math::Matrix& mOrthographicProjection);
		void UpdateMiscBuffer(const std::vector<Light*>& lights, Camera* camera);
		void UpdateTextures(std::vector<void*> textures);
		void Set();
		void Render(int indexCount);
		bool IsCompiled();

	private:

		struct MatrixBufferType
		{
			Math::Matrix worldViewProjection;
			Math::Matrix mProjection;
			Math::Matrix mProjectionInverse;
			Math::Matrix mViewProjection;
			Math::Matrix mViewProjectionInverse;
			Math::Matrix mView;
		};

		const static int maxLights = 64;
		struct MiscBufferType
		{
			Math::Vector4 cameraPosition;
			
			//= DIRECTIONAL LIGHT ==========	
			Math::Vector4 dirLightColor;
			Math::Vector4 dirLightIntensity;
			Math::Vector4 dirLightDirection;
			//==============================

			//= POINT LIGHTS =============================
			Math::Vector4 pointLightPosition[maxLights];
			Math::Vector4 pointLightColor[maxLights];
			Math::Vector4 pointLightIntenRange[maxLights];
			//============================================

			//= SPOT LIGHTS ==================================
			Math::Vector4 spotLightColor[maxLights];
			Math::Vector4 spotLightPosition[maxLights];
			Math::Vector4 spotLightDirection[maxLights];
			Math::Vector4 spotLightIntenRangeAngle[maxLights];
			//================================================

			float pointLightCount;
			float spotLightCount;
			float nearPlane;
			float farPlane;
			Math::Vector2 viewport;
			Math::Vector2 padding;
		};

		std::shared_ptr<D3D11ConstantBuffer> m_matrixBuffer;
		std::shared_ptr<D3D11ConstantBuffer> m_miscBuffer;
		std::shared_ptr<D3D11Shader> m_shader;
		Graphics* m_graphics;
	};
}