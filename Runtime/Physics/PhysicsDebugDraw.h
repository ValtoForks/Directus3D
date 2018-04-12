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

//= INCLUDES ==========================
// Hide warnings which belong to Bullet
#pragma warning(push, 0)   
#include <LinearMath/btIDebugDraw.h>
#pragma warning(pop)
#include <vector>
#include "../Graphics/Vertex.h"
//=====================================

namespace Directus
{
	class PhysicsDebugDraw : public btIDebugDraw
	{
		int m_debugMode;

	public:
		PhysicsDebugDraw();
		void Release();

		virtual ~PhysicsDebugDraw();
		//= btIDebugDraw ==============================================================================================================================
		void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		void drawSphere(const btVector3& p, btScalar radius, const btVector3& color) override;
		void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha) override;
		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
		void reportErrorWarning(const char* warningString) override;
		void draw3dText(const btVector3& location, const char* textString) override;
		void setDebugMode(int debugMode) override;
		int getDebugMode() const override;
		//=============================================================================================================================================

		bool IsDirty();
		const std::vector<VertexPosCol>& GetLines();
		void Clear();
	private:
		std::vector<VertexPosCol> m_lines;
		bool m_isDirty;
	};
}