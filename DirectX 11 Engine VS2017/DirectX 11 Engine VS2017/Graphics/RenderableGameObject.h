#pragma once
#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);

	void Draw(const XMMATRIX & viewProjectionMatrix);

private:
	Model model;
	void UpdateMatrix() override;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};

