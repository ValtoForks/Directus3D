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

//= INCLUDES ========================
#include <string>
#include "../ImGui/imgui.h"
#include "Math/Vector4.h"
#include "Math/Vector2.h"
#include "RHI/RHI_Implementation.h"
#include "Resource/ResourceManager.h"
#include "Core/Engine.h"
#include "FileSystem/FileSystem.h"
#include "Threading/Threading.h"
#include "World/World.h"
#include "RHI/RHI_Texture.h"
//===================================

// An icon shader resource pointer by thumbnail
#define SHADER_RESOURCE_BY_THUMBNAIL(thumbnail)			IconProvider::Get().GetShaderResourceByThumbnail(thumbnail)
// An icon shader resource pointer by type 
#define SHADER_RESOURCE_BY_TYPE(type)					IconProvider::Get().GetShaderResourceByType(type)
// An thumbnail button by thumbnail
#define THUMBNAIL_BUTTON(thumbnail, size)				ImGui::ImageButton(SHADER_RESOURCE(thumbnail), ImVec2(size, size))
// An thumbnail button by enum
#define THUMBNAIL_BUTTON_BY_TYPE(type, size)			ImGui::ImageButton(SHADER_RESOURCE_BY_TYPE(type), ImVec2(size, size))
// An thumbnail button by enum, with a specific ID
#define THUMBNAIL_BUTTON_TYPE_UNIQUE_ID(id, type, size)	IconProvider::Get().ImageButton_enum_id(id, type, size)

// A thumbnail image
#define THUMBNAIL_IMAGE(thumbnail, size)							\
	ImGui::Image(													\
	IconProvider::Get().GetShaderResourceByThumbnail(thumbnail),	\
	ImVec2(size, size),												\
	ImVec2(0, 0),													\
	ImVec2(1, 1),													\
	ImColor(255, 255, 255, 255),									\
	ImColor(255, 255, 255, 0))										\

// A thumbnail image by shader resource
#define THUMBNAIL_IMAGE_BY_SHADER_RESOURCE(srv, size)	\
	ImGui::Image(										\
	srv,												\
	ImVec2(size, size),									\
	ImVec2(0, 0),										\
	ImVec2(1, 1),										\
	ImColor(255, 255, 255, 255),						\
	ImColor(255, 255, 255, 0))							\

// A thumbnail image by enum
#define THUMBNAIL_IMAGE_BY_ENUM(type, size)	\
	ImGui::Image(							\
	SHADER_RESOURCE_BY_TYPE(type),			\
	ImVec2(size, size),						\
	ImVec2(0, 0),							\
	ImVec2(1, 1),							\
	ImColor(255, 255, 255, 255),			\
	ImColor(255, 255, 255, 0))				\

class EditorHelper
{
public:
	EditorHelper()
	{
		m_context			= nullptr;	
		m_engine			= nullptr;
		m_resourceManager	= nullptr;
		m_scene				= nullptr;
	}
	~EditorHelper(){}

	static EditorHelper& Get()
	{
		static EditorHelper instance;
		return instance;
	}

	void Initialize(Directus::Context* context)
	{
		m_context			= context;
		m_engine			= m_context->GetSubsystem<Directus::Engine>();
		m_resourceManager	= m_context->GetSubsystem<Directus::ResourceManager>();
		m_scene				= m_context->GetSubsystem<Directus::World>();
	}

	std::weak_ptr<Directus::RHI_Texture> GetOrLoadTexture(const std::string& filePath, bool async = false)
	{
		// Validate file path
		if (Directus::FileSystem::IsDirectory(filePath))
			return std::weak_ptr<Directus::RHI_Texture>();
		if (!Directus::FileSystem::IsSupportedImageFile(filePath) && !Directus::FileSystem::IsEngineTextureFile(filePath))
			return std::weak_ptr<Directus::RHI_Texture>();

		// Compute some useful information
		auto path = Directus::FileSystem::GetRelativeFilePath(filePath);
		auto name = Directus::FileSystem::GetFileNameNoExtensionFromFilePath(path);

		// Check if this texture is already cached, if so return the cached one
		auto resourceManager = m_context->GetSubsystem<Directus::ResourceManager>();	
		if (auto cached = resourceManager->GetResourceByName<Directus::RHI_Texture>(name))		
			return cached;

		// Since the texture is not cached, load it and returned a cached ref
		auto texture = std::make_shared<Directus::RHI_Texture>(m_context);
		texture->SetResourceName(name);
		texture->SetResourceFilePath(path);
		if (!async)
		{
			texture->LoadFromFile(path);
		}
		else
		{
			m_context->GetSubsystem<Directus::Threading>()->AddTask([texture, filePath]()
			{
				texture->LoadFromFile(filePath);
			});
		}

		return texture->Cache<Directus::RHI_Texture>();
	}

	void LoadModel(const std::string& filePath)
	{
		// Load the model asynchronously
		m_context->GetSubsystem<Directus::Threading>()->AddTask([this, filePath]()
		{
			m_resourceManager->Load<Directus::Model>(filePath);
		});
	}

	void LoadScene(const std::string& filePath)
	{
		// Load the scene asynchronously
		m_context->GetSubsystem<Directus::Threading>()->AddTask([this, filePath]()
		{
			m_scene->LoadFromFile(filePath);
		});
	}

	void SaveScene(const std::string& filePath)
	{
		// Save the scene asynchronously
		m_context->GetSubsystem<Directus::Threading>()->AddTask([this, filePath]()
		{
			m_scene->SaveToFile(filePath);
		});
	}

	//= CONVERSIONS ===================================================================================================
	static Directus::Math::Vector4 ToVector4(const ImVec4& v)	{ return Directus::Math::Vector4(v.x, v.y, v.z, v.w); }
	static Directus::Math::Vector2 ToVector2(const ImVec2& v)	{ return Directus::Math::Vector2{ v.x,v.y }; }
	//=================================================================================================================

private:
	Directus::Context* m_context;
	Directus::Engine* m_engine;
	Directus::ResourceManager* m_resourceManager;
	Directus::World* m_scene;
};