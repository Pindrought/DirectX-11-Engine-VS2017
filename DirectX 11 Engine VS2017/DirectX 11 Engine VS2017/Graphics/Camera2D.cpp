#include "Camera2D.h"

Camera2D::Camera2D()
{
	pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	posVector = XMLoadFloat3(&pos);
	rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotVector = XMLoadFloat3(&rot);
	UpdateMatrix();
}

void Camera2D::SetProjectionValues(float width, float height, float nearZ, float farZ)
{
	orthoMatrix = XMMatrixOrthographicLH(width, height, nearZ, farZ);
}

const XMMATRIX & Camera2D::GetOrthoMatrix() const
{
	return orthoMatrix;
}

const XMMATRIX & Camera2D::GetWorldMatrix() const
{
	return worldMatrix;
}

void Camera2D::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	XMMATRIX translationOffsetMatrix = XMMatrixTranslation(-pos.x, -pos.y, 0.0f); //z component irrelevant for 2d camera
	//Calculate camera rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z); 
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	worldMatrix = translationOffsetMatrix * camRotationMatrix;
}