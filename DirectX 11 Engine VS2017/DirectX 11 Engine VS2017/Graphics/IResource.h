#pragma once
#include <string>

enum ResourceType
{
	RT_Unknown, //Keep RT_Unknown as first resource type for initialization of maps in Resource Manager
	RT_Texture,
	RT_PixelShader,
	RT_VertexShader,
	RT_Model,
	RT_End //Keep RT_End as last resource type for initialization of maps in Resource Manager
};

class IResource
{
public:
	IResource(ResourceType type = ResourceType::RT_Unknown, std::string name = "UNINITIALIZED");
	ResourceType GetResourceType();
	const std::string & GetResourceName();
protected:
	ResourceType type = ResourceType::RT_Unknown;
	std::string name = "";
};