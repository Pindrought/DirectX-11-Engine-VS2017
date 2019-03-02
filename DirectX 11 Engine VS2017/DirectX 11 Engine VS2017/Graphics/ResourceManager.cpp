#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	//Initialize maps for the resource types
	for (int i = ResourceType::RT_Unknown; i < ResourceType::RT_End; i++)
	{
		std::map<std::wstring, std::shared_ptr<IResource>> newMap;
		resourceMap.insert(std::make_pair(static_cast<ResourceType>(i), newMap));
	}
}

void ResourceManager::RegisterResource(std::shared_ptr<IResource> resource)
{
	resourceMap[resource->GetResourceType()][resource->GetResourceName()] = resource;
}

std::shared_ptr<IResource> ResourceManager::GetResource(ResourceType type, std::wstring name)
{
	std::shared_ptr<IResource> resource = nullptr;
	std::map<std::wstring, std::shared_ptr<IResource>>::iterator it;
	it = resourceMap[type].find(name);
	if (it != resourceMap[type].end()) //If resource was found
		return it->second; //return ptr to resource retrieved from map

	return nullptr; //return nullptr if resource was not found
}

std::map<ResourceType, std::map<std::wstring, std::shared_ptr<IResource>>> ResourceManager::resourceMap;

ResourceManager rm; //Initialize an instance of a resource manager to call static constructor

