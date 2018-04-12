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

//= INCLUDES =====================
#include <map>
#include "Scene.h"
#include "Components/IComponent.h"
#include "../Core/Context.h"
#include "../Core/EventSystem.h"
//================================

namespace Directus
{
	class Transform;
	class Renderable;

	class ENGINE_CLASS GameObject : public std::enable_shared_from_this<GameObject>
	{
	public:
		GameObject(Context* context);
		~GameObject();

		void Initialize(Transform* transform);

		//============
		void Start();
		void Stop();
		void Update();
		//============

		bool SaveAsPrefab(const std::string& filePath);
		bool LoadFromPrefab(const std::string& filePath);

		void Serialize(FileStream* stream);
		void Deserialize(FileStream* stream, Transform* parent);

		//= PROPERTIES =======================================================================================
		const std::string& GetName() { return m_name; }
		void SetName(const std::string& name) { m_name = name; }

		unsigned int GetID() { return m_ID; }
		void SetID(unsigned int ID) { m_ID = ID; }

		bool IsActive() { return m_isActive; }
		void SetActive(bool active) { m_isActive = active; }

		bool IsVisibleInHierarchy() { return m_hierarchyVisibility; }
		void SetHierarchyVisibility(bool hierarchyVisibility) { m_hierarchyVisibility = hierarchyVisibility; }
		//====================================================================================================

		//= COMPONENTS =========================================================================================
		// Adds a component of type T
		template <class T>
		std::weak_ptr<T> AddComponent()
		{
			ComponentType type = IComponent::Type_To_Enum<T>();

			// Return component in case it already exists while ignoring Script components (they can exist multiple times)
			if (HasComponent(type) && type != ComponentType_Script)
				return std::static_pointer_cast<T>(GetComponent<T>().lock());

			// Add component
			auto newComponent = std::make_shared<T>(
				m_context, 
				m_context->GetSubsystem<Scene>()->GetGameObjectByID(GetID()).lock().get(),
				GetTransform_PtrRaw()
				);
			m_components.insert(make_pair(type, newComponent));
			newComponent->SetType(IComponent::Type_To_Enum<T>());

			// Register component
			newComponent->OnInitialize();

			// Caching of rendering performance critical components
			if (newComponent->GetType() == ComponentType_Renderable)
			{
				m_renderable = (Renderable*)newComponent.get();
			}

			// Make the scene resolve
			FIRE_EVENT(EVENT_SCENE_RESOLVE);

			// Return it as a component of the requested type
			return newComponent;
		}

		std::weak_ptr<IComponent> AddComponent(ComponentType type);

		// Returns a component of type T (if it exists)
		template <class T>
		std::weak_ptr<T> GetComponent()
		{
			ComponentType type = IComponent::Type_To_Enum<T>();

			if (m_components.find(type) == m_components.end())
				return std::weak_ptr<T>();

			return std::static_pointer_cast<T>(m_components.find(type)->second);
		}

		// Returns any components of type T (if they exist)
		template <class T>
		std::vector<std::weak_ptr<T>> GetComponents()
		{
			ComponentType type = IComponent::Type_To_Enum<T>();

			std::vector<std::weak_ptr<T>> components;
			for (const auto& component : m_components)
			{
				if (type != component.second->GetType())
					continue;

				components.emplace_back(std::static_pointer_cast<T>(component.second));
			}

			return components;
		}
		
		// Checks if a component of ComponentType exists
		bool HasComponent(ComponentType type) { return m_components.find(type) != m_components.end(); }
		// Checks if a component of type T exists
		template <class T>
		bool HasComponent() { return HasComponent(IComponent::Type_To_Enum<T>()); }

		// Removes a component of type T (if it exists)
		template <class T>
		void RemoveComponent()
		{
			ComponentType type = IComponent::Type_To_Enum<T>();

			if (m_components.find(type) == m_components.end())
				return;

			for (auto it = m_components.begin(); it != m_components.end(); )
			{
				auto component = *it;
				if (type == component.second->GetType())
				{
					component.second->OnRemove();
					component.second.reset();
					it = m_components.erase(it);
				}
				else
				{
					++it;
				}
			}

			// Make the scene resolve
			FIRE_EVENT(EVENT_SCENE_RESOLVE);
		}

		void RemoveComponentByID(unsigned int id);
		//======================================================================================================

		// Direct access for performance critical usage (not safe)
		Transform* GetTransform_PtrRaw() { return m_transform; }
		Renderable* GetRenderable_PtrRaw() { return m_renderable; }
		std::shared_ptr<GameObject> GetPtrShared() { return shared_from_this(); }

	private:
		unsigned int m_ID;
		std::string m_name;
		bool m_isActive;
		bool m_isPrefab;
		bool m_hierarchyVisibility;
		std::multimap<ComponentType, std::shared_ptr<IComponent>> m_components;
		Context* m_context;

		// Caching of performance critical components
		Transform* m_transform;
		Renderable* m_renderable;
	};
}
