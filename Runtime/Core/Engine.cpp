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

//= INCLUDES ===========================
#include "Engine.h"
#include "Timer.h"
#include "Settings.h"
#include "Stopwatch.h"
#include "../Rendering/Renderer.h"
#include "../Core/EventSystem.h"
#include "../Logging/Log.h"
#include "../Threading/Threading.h"
#include "../Resource/ResourceManager.h"
#include "../Scripting/Scripting.h"
#include "../Audio/Audio.h"
#include "../Physics/Physics.h"
#include "../World/World.h"
#include "../Profiling/Profiler.h"
#include "../Input/Input.h"
//======================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Directus
{
	void* Engine::m_drawHandle		= nullptr;
	void* Engine::m_windowHandle	= nullptr;
	void* Engine::m_windowInstance	= nullptr;
	unsigned long Engine::m_flags	= 0;
	static unique_ptr<Stopwatch> g_stopwatch;

	Engine::Engine(Context* context) : Subsystem(context)
	{
		m_flags |= Engine_Update;
		m_flags |= Engine_Render;
		m_flags |= Engine_Physics;
		m_flags |= Engine_Game;

		m_timer			= nullptr;
		g_stopwatch		= make_unique<Stopwatch>();

		// Register self as a subsystem
		m_context->RegisterSubsystem(this);

		// Initialize global/static subsystems 
		Log::Initialize();
		FileSystem::Initialize();
		Settings::Get().Initialize();

		// Register subsystems
		m_context->RegisterSubsystem(new Timer(m_context));
		m_context->RegisterSubsystem(new Input(m_context));
		m_context->RegisterSubsystem(new Threading(m_context));
		m_context->RegisterSubsystem(new ResourceManager(m_context));
		m_context->RegisterSubsystem(new Renderer(m_context, m_drawHandle));
		m_context->RegisterSubsystem(new Audio(m_context));
		m_context->RegisterSubsystem(new Physics(m_context));
		m_context->RegisterSubsystem(new Scripting(m_context));
		m_context->RegisterSubsystem(new World(m_context));
	}

	bool Engine::Initialize()
	{
		// Timer
		m_timer = m_context->GetSubsystem<Timer>();
		if (!m_timer->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize");
			return false;
		}
	
		// Input
		if (!m_context->GetSubsystem<Input>()->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize Input");
			return false;
		}

		// Threading
		if (!m_context->GetSubsystem<Threading>()->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize Multithreading");
			return false;
		}

		// ResourceManager
		if (!m_context->GetSubsystem<ResourceManager>()->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize ResourceManager");
			return false;
		}

		// Renderer
		if (!m_context->GetSubsystem<Renderer>()->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize Renderer");
			return false;
		}

		// Audio
		if (!m_context->GetSubsystem<Audio>()->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize Audio");
			return false;
		}

		// Physics
		if (!m_context->GetSubsystem<Physics>()->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize Physics");
			return false;
		}

		// Scripting
		if (!m_context->GetSubsystem<Scripting>()->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize Scripting");
			return false;
		}

		// Scene
		if (!m_context->GetSubsystem<World>()->Initialize())
		{
			LOG_ERROR("Engine::Initialize: Failed to initialize Scene");
			return false;
		}

		Profiler::Get().Initialize(m_context);
		g_stopwatch->Start();

		return true;
	}

	void Engine::Tick()
	{
		m_timer->Tick();
		FIRE_EVENT(EVENT_FRAME_START);

		if (EngineMode_IsSet(Engine_Update))
		{
			FIRE_EVENT_DATA(EVENT_TICK, m_timer->GetDeltaTimeSec());
		}

		if (EngineMode_IsSet(Engine_Render))
		{
			FIRE_EVENT(EVENT_RENDER);
		}

		FIRE_EVENT(EVENT_FRAME_END);
	}

	void Engine::Shutdown()
	{
		// The context will deallocate the subsystems
		// in the reverse order in which they were registered.
		SafeDelete(m_context);

		// Release Log singleton
		Log::Release();
	}

	void Engine::SetHandles(void* drawHandle, void* windowHandle, void* windowInstance)
	{
		m_drawHandle		= drawHandle;
		m_windowHandle		= windowHandle;
		m_windowInstance	= windowInstance;
	}

	float Engine::GetDeltaTime()
	{
		return m_timer->GetDeltaTimeSec();
	}
}