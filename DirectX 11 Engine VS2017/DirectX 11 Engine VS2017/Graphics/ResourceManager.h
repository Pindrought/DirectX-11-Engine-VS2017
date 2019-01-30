#pragma once
#include "IResource.h"
#include <vector>
#include <map>

static class ResourceManager
{
public:
	ResourceManager();
	static void RegisterResource(IResource * resource);
	static IResource * GetResource(ResourceType type, std::string name);
private:
	static std::map<ResourceType, std::map<std::string, IResource*>> resourceMap;
};