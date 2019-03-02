#include "RenderableGameObject3D.h"
#include "ResourceManager.h"

bool RenderableGameObject3D::Initialize(const std::string & filePath, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	std::shared_ptr<IResource> modelPtr = ResourceManager::GetResource(ResourceType::RT_Model, StringHelper::GetFileNameOnlyFromPath(StringHelper::StringToWide(filePath)));

	if (modelPtr != nullptr) //if model was successfully retrieved from resource manager
	{
		model = std::static_pointer_cast<Model>(modelPtr);
	}
	else //If model needs to be loaded in
	{
		model = std::make_shared<Model>();
		if (!model->Initialize(filePath, cb_vs_vertexshader))
			return false;
		ResourceManager::RegisterResource(std::static_pointer_cast<IResource>(model));
	}
	
	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	UpdateMatrix();
	return true;
}

void RenderableGameObject3D::Draw(const XMMATRIX & viewProjectionMatrix)
{
	model->Draw(worldMatrix, viewProjectionMatrix);
}

void RenderableGameObject3D::UpdateMatrix()
{
	worldMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x, pos.y, pos.z);
	UpdateDirectionVectors();
}