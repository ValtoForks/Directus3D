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

//= INCLUDES ==================
#include "../Core/EngineDefs.h"
#include "../Core/Settings.h"
#include "RHI_Object.h"
//=============================

namespace Directus
{
	class ENGINE_CLASS RHI_Viewport : public RHI_Object
	{
	public:
		RHI_Viewport
		(
			float topLeftX	= 0.0f,
			float topLeftY	= 0.0f,
			float width		= (float)Settings::Get().Resolution_GetWidth(),
			float height	= (float)Settings::Get().Resolution_GetHeight(),
			float minDepth	= 0.0f,
			float maxDepth	= 1.0f
		)
		{
			m_topLeftX	= topLeftX;
			m_topLeftY	= topLeftY;
			m_width		= width;
			m_height	= height;
			m_minDepth	= minDepth;
			m_maxDepth	= maxDepth;
		}

		~RHI_Viewport(){}

		bool operator==(const RHI_Viewport& rhs) const
		{
			return 
				m_topLeftX	== rhs.m_topLeftX	&& m_topLeftY	== rhs.m_topLeftY && 
				m_width		== rhs.m_width		&& m_height		== rhs.m_height && 
				m_minDepth	== rhs.m_minDepth	&& m_maxDepth	== rhs.m_maxDepth;
		}

		bool operator!=(const RHI_Viewport& rhs) const
		{
			return !(*this == rhs);
		}

		float GetTopLeftX() const		{ return m_topLeftX; }
		float GetTopLeftY() const		{ return m_topLeftY; }
		float GetWidth()	const		{ return m_width; }
		float GetHeight() const			{ return m_height; }
		float GetMinDepth() const		{ return m_minDepth; }
		float GetMaxDepth() const		{ return m_maxDepth; }

		void SetWidth(float width)		{ m_width = width; }
		void SetHeight(float height)	{ m_height = height; }

	private:
		float m_topLeftX;
		float m_topLeftY;
		float m_width;
		float m_height;
		float m_minDepth;
		float m_maxDepth;
	};
}