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

//= INCLUDES ========================
#include "Widget_Viewport.h"
#include "../../ImGui/Source/imgui.h"
#include "Rendering/Renderer.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Components/Camera.h"
#include "../EditorHelper.h"
#include "Widget_Scene.h"
#include "../DragDrop.h"
//===================================

//= NAMESPACES ==========
using namespace std;
using namespace Directus;
using namespace Math;
//=======================

static Renderer* g_renderer			= nullptr;
static Scene* g_scene				= nullptr;
static Vector2 g_framePos;

static bool g_physics				= true;
static bool g_aabb					= false;
static bool g_gizmos				= true;
static bool g_pickingRay			= false;
static bool g_grid					= true;
static bool g_performanceMetrics	= false;

Widget_Viewport::Widget_Viewport()
{
	m_title = "Viewport";
}

void Widget_Viewport::Initialize(Context* context)
{
	Widget::Initialize(context);
	m_windowFlags	|= ImGuiWindowFlags_NoScrollbar;
	g_renderer		= m_context->GetSubsystem<Renderer>();
	g_scene			= m_context->GetSubsystem<Scene>();
}

void Widget_Viewport::Update()
{
	if (!g_renderer)
		return;
	
	ShowTopToolbar();
	ImGui::Separator();
	ShowFrame();
}

void Widget_Viewport::ShowTopToolbar()
{
	// Render options
	ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x); ImGui::Checkbox("Physics", &g_physics);
	ImGui::SameLine(); ImGui::Checkbox("AABB", &g_aabb);
	ImGui::SameLine(); ImGui::Checkbox("Gizmos", &g_gizmos);
	ImGui::SameLine(); ImGui::Checkbox("Picking Ray", &g_pickingRay);
	ImGui::SameLine(); ImGui::Checkbox("Scene Grid", &g_grid);
	ImGui::SameLine(); ImGui::Checkbox("Performance Metrics", &g_performanceMetrics);

	g_physics				? Renderer::RenderFlags_Enable(Render_Physics)			: Renderer::RenderFlags_Disable(Render_Physics);
	g_aabb					? Renderer::RenderFlags_Enable(Render_AABB)				: Renderer::RenderFlags_Disable(Render_AABB);
	g_gizmos				? Renderer::RenderFlags_Enable(Render_Light)			: Renderer::RenderFlags_Disable(Render_Light);
	g_pickingRay			? Renderer::RenderFlags_Enable(Render_PickingRay)		: Renderer::RenderFlags_Disable(Render_PickingRay);
	g_grid					? Renderer::RenderFlags_Enable(Render_SceneGrid)			: Renderer::RenderFlags_Disable(Render_SceneGrid);
	g_performanceMetrics	? Renderer::RenderFlags_Enable(Render_PerformanceMetrics)	: Renderer::RenderFlags_Disable(Render_PerformanceMetrics);
}

void Widget_Viewport::ShowFrame()
{
	float width		= ImGui::GetWindowContentRegionWidth();
	float height	= ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y - 30;

	g_renderer->SetResolutionInternal((int)width, (int)height);

	g_framePos = EditorHelper::ToVector2(ImGui::GetCursorPos()) + EditorHelper::ToVector2(ImGui::GetWindowPos());
	ImGui::Image(
		g_renderer->GetFrame(),
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
	// Handle model drop
	if (auto payload = DragDrop::Get().GetPayload(DragPayload_Model))
	{
		EditorHelper::Get().LoadModel(get<string>(payload->data));
	}

	MousePicking();
}

void Widget_Viewport::MousePicking()
{
	if (!ImGui::IsMouseHoveringWindow() || !ImGui::IsMouseClicked(0))
		return;

	auto camera = g_scene->GetMainCamera();
	if (!camera.expired())
	{
		Vector2 mousePosRelative = EditorHelper::ToVector2(ImGui::GetMousePos()) - g_framePos;
		auto picked = camera.lock()->GetComponent<Camera>().lock()->Pick(mousePosRelative);
		if (!picked.expired())
		{
			Widget_Scene::SetSelectedActor(picked);
			return;
		}
	}

	Widget_Scene::SetSelectedActor(weak_ptr<Actor>());
}