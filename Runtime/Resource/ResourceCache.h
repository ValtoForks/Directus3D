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

//= INCLUDES ==============
#include <vector>
#include <memory>
#include "IResource.h"
#include "../Logging/Log.h"
#include <mutex>
//========================

namespace Directus
{
	class ENGINE_CLASS ResourceCache
	{
	public:
		ResourceCache() {}
		~ResourceCache() { Clear(); }

		// Adds a resource
		void Add(const std::shared_ptr<IResource>& resource)
		{
			if (!resource)
				return;

			std::lock_guard<std::mutex> guard(m_mutex);
			m_resourceGroups[resource->GetResourceType()].push_back(resource);
		}

		// Returns the file paths of all the resources
		void GetResourceFilePaths(std::vector<std::string>& filePaths)
		{
			for (const auto& resourceGroup : m_resourceGroups)
			{
				for (const auto& resource : resourceGroup.second)
				{
					filePaths.push_back(resource->GetResourceFilePath());
				}
			}
		}

		// Makes the resources save their metadata
		void SaveResourcesToFiles()
		{
			for (const auto& resourceGroup : m_resourceGroups)
			{
				for (const auto& resource : resourceGroup.second)
				{
					if (!resource->HasFilePath())
						continue;

					resource->SaveToFile(resource->GetResourceFilePath());
				}
			}
		}

		// Returns all the resources
		std::vector<std::shared_ptr<IResource>> GetAll()
		{
			std::vector<std::shared_ptr<IResource>> resources;
			for (const auto& resourceGroup : m_resourceGroups)
			{
				resources.insert(resources.end(), resourceGroup.second.begin(), resourceGroup.second.end());
			}

			return resources;
		}

		// Returns a resource by name
		template <class T>
		std::shared_ptr<IResource> GetByName(const std::string& name)
		{
			for (const auto& resource : m_resourceGroups[IResource::DeduceResourceType<T>()])
			{
				if (name == resource->GetResourceName())
					return resource;
			}

			return std::shared_ptr<IResource>();
		}

		// Returns a resource by path
		template <class T>
		std::shared_ptr<IResource> GetByPath(const std::string& path)
		{
			for (const auto& resource : m_resourceGroups[IResource::DeduceResourceType<T>()])
			{
				if (path == resource->GetResourceFilePath())
					return resource;
			}

			return std::shared_ptr<IResource>();
		}

		// Checks whether a resource is already cached
		bool IsCached(const std::shared_ptr<IResource>& resourceIn)
		{
			return IsCached(resourceIn->GetResourceName(), resourceIn->GetResourceType());
		}

		// Checks whether a resource is already cached
		bool IsCached(const std::string& resourceName, ResourceType resourceType)
		{
			if (resourceName == NOT_ASSIGNED)
			{
				LOG_WARNING("ResourceCache:IsCached: Can't check if resource \"" + resourceName + "\" is cached as it has no name assigned to it.");
				return false;
			}

			for (const auto& resource : m_resourceGroups[resourceType])
			{
				if (resourceName == resource->GetResourceName())
					return true;
			}

			return false;
		}

		unsigned int GetMemoryUsage()
		{
			unsigned int size = 0;
			for (const auto& group : m_resourceGroups)
			{
				for (const auto& resource : group.second)
				{
					if (!resource)
						continue;

					size += resource->GetMemory();
				}
			}

			return size;
		}

		unsigned int GetMemoryUsage(ResourceType type)
		{
			unsigned int size = 0;
			for (const auto& resource : m_resourceGroups[type])
			{
				size += resource->GetMemory();
			}

			return size;
		}

		// Returns all resources of a given type
		const std::vector<std::shared_ptr<IResource>>& GetByType(ResourceType type) { return m_resourceGroups[type]; }

		// Unloads all resources
		void Clear() { m_resourceGroups.clear(); }

	private:
		std::map<ResourceType, std::vector<std::shared_ptr<IResource>>> m_resourceGroups;
		std::mutex m_mutex;
	};
}