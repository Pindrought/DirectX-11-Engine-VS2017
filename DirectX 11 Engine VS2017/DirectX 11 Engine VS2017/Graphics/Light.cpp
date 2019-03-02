#include "Light.h"
#include "ResourceManager.h"
#include "PipelineManager.h"

bool Light::Initialize(ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	std::shared_ptr<IResource> modelPtr = ResourceManager::GetResource(ResourceType::RT_Model, L"Data/Objects/light.fbx");

	if (modelPtr != nullptr) //if model was successfully retrieved from resource manager
	{
		model = std::static_pointer_cast<Model>(modelPtr);
	}
	else //If model needs to be loaded in
	{
		model = std::make_shared<Model>();
		if (!model->Initialize("Data/Objects/light.fbx", cb_vs_vertexshader))
			return false;
		ResourceManager::RegisterResource(std::static_pointer_cast<IResource>(model));
	}

	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	UpdateMatrix();
	return true;
}