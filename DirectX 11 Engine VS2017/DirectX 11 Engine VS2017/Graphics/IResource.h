#pragma once
#include <string>

enum ResourceType
{
	RT_Unknown, //Keep RT_Unknown as first resource type for initialization of maps in Resource Manager
	RT_Texture,
	RT_Model,
	RT_End //Keep RT_End as last resource type for initialization of maps in Resource Manager
};

class IResource
{
public:
	IResource(ResourceType type = ResourceType::RT_Unknown, std::wstring name = L"UNINITIALIZED");
	ResourceType GetResourceType();
	const std::wstring & GetResourceName();
protected:
	ResourceType type = ResourceType::RT_Unknown;
	std::wstring name = L"";
};