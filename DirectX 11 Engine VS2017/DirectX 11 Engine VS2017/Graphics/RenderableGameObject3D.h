#pragma once
#include "GameObject3D.h"
#include "Model.h"

class RenderableGameObject3D : public GameObject3D
{
public:
	bool Initialize(const std::string & filePath, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);

	void Draw(const XMMATRIX & viewProjectionMatrix);

protected:
	std::shared_ptr<Model> model = nullptr;
	void UpdateMatrix() override;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};