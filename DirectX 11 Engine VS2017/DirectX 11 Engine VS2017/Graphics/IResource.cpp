#include "IResource.h"

IResource::IResource(ResourceType type, std::string name)
	:type(type), name(name)
{
}

ResourceType IResource::GetResourceType()
{
	return type;
}

const std::string & IResource::GetResourceName()
{
	return name;
}
