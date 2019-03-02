#pragma once
#include "IResource.h"
#include <vector>
#include <map>
#include <wrl/client.h>

static class ResourceManager
{
public:
	ResourceManager();
	static void RegisterResource(std::shared_ptr<IResource> resource);
	static std::shared_ptr<IResource> GetResource(ResourceType type, std::wstring name);
private:
	static std::map<ResourceType, std::map<std::wstring, std::shared_ptr<IResource>>> resourceMap;
};