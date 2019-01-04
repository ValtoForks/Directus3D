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

//= INCLUDES ==========
#include <vector>
#include "EngineDefs.h"
#include "SubSystem.h"
//=====================

namespace Directus
{
	class ENGINE_CLASS Context
	{
	public:
		Context() {}

		~Context()
		{
			for (auto i = m_subsystems.size() - 1; i > 0; i--)
				delete m_subsystems[i];

			// Index 0 is the actual Engine instance, which is the instance
			// that called this destructor in the first place. A deletion
			// will result in a crash.
		}

		// Register a subsystem
		void RegisterSubsystem(Subsystem* subsystem)
		{
			if (!subsystem)
				return;

			m_subsystems.emplace_back(subsystem);
		}

		// Get a subsystem
		template <class T> T* GetSubsystem();
	private:
		std::vector<Subsystem*> m_subsystems;
	};

	template <class T>
	T* Context::GetSubsystem()
	{
		// Compare T with subsystem types
		for (const auto& subsystem : m_subsystems)
		{
			if (typeid(T) == typeid(*subsystem))
				return static_cast<T*>(subsystem);
		}

		return nullptr;
	}
}