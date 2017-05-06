/* Copyright (c) <2016> <Panos Karabelas>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE. */

#pragma once

//= INCLUDES =========
#include "Subsystem.h"
#include <vector>
//====================

namespace Directus
{
	class DllExport Context
	{
	public:
		Context();
		~Context();

		// Register a subsystem
		void RegisterSubsystem(Subsystem* subsystem);

		// Get a subsystem
		template <class T> T* GetSubsystem();
	private:
		std::vector<Subsystem*> m_subsystems;
	};

	template <class T>
	T* Context::GetSubsystem()
	{
		for (auto subsystem : m_subsystems)
		{
			// casting failure results in nullptr
			T* typedSubsystem = dynamic_cast<T*>(subsystem);


			if (typedSubsystem)
				return typedSubsystem;
		}

		return nullptr;
	}
}