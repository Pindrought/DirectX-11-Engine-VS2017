#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	for (int i = ResourceType::RT_Unknown; i < ResourceType::RT_End; i++)
	{
		std::map<std::string, IResource*> newMap;
		resourceMap.insert(std::make_pair(static_cast<ResourceType>(i), newMap));
	}
}

void ResourceManager::RegisterResource(IResource * resource)
{
	resourceMap[resource->GetResourceType()][resource->GetResourceName] = resource;
}

IResource * ResourceManager::GetResource(ResourceType type, std::string name)
{
	IResource * resource = nullptr;
	std::map<std::string, IResource *>::iterator it;
	it = resourceMap[type].find(name);
	if (it != resourceMap[type].end()) //If resource was found
		return it->second; //return ptr to resource retrieved from map

	return nullptr; //return nullptr if resource was not found
}
