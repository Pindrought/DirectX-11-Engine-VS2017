#include "IResource.h"

IResource::IResource(ResourceType type, std::wstring name)
	:type(type), name(name)
{
}

ResourceType IResource::GetResourceType()
{
	return type;
}

const std::wstring & IResource::GetResourceName()
{
	return name;
}
