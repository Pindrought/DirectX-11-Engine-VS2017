#include "Camera3D.h"

Camera3D::Camera3D()
{
	pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	posVector = XMLoadFloat3(&pos);
	rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotVector = XMLoadFloat3(&rot);
	UpdateMatrix();
}

void Camera3D::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX & Camera3D::GetViewMatrix() const
{
	return viewMatrix;
}

const XMMATRIX & Camera3D::GetProjectionMatrix() const
{
	return projectionMatrix;
}

void Camera3D::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	//Calculate camera rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	XMVECTOR camTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//Adjust cam target to be offset by the camera's current position
	camTarget += posVector;
	//Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
	//Rebuild view matrix
	viewMatrix = XMMatrixLookAtLH(posVector, camTarget, upDir);

	UpdateDirectionVectors();
}