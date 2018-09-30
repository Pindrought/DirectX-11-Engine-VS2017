#include "Camera.h"
Camera::Camera()
{
	this->pos.m128_f32[0] = 0;
	this->pos.m128_f32[1] = 0;
	this->pos.m128_f32[2] = 0;
	this->pos.m128_f32[3] = 0;
	this->rot.x = 0;
	this->rot.y = 0;
	this->rot.z = 0;
	UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
	this->pos.m128_f32[0] = x;
	this->pos.m128_f32[1] = y;
	this->pos.m128_f32[2] = z;
	this->pos.m128_f32[3] = 0;
	UpdateViewMatrix();
}

void Camera::AdjustPosition(XMVECTOR posOffset)
{
	this->pos += posOffset;
	this->pos.m128_f32[3] = 0;
	UpdateViewMatrix();
}

void Camera::SetRotation(float xRot, float yRot, float zRot)
{
	this->rot.x = xRot / 180 * 3.14159;
	this->rot.y = yRot / 180 * 3.14159;
	this->rot.z = zRot / 180 * 3.14159;
	UpdateViewMatrix();
}

void Camera::AdjustRotation(float xRotOffset, float yRotOffset, float zRotOffset)
{
	this->rot.x += xRotOffset;
	this->rot.y += yRotOffset;
	this->rot.z += zRotOffset;
	if (this->rot.x < -XM_PIDIV2)
		this->rot.x = -XM_PIDIV2;
	else if (this->rot.x > XM_PIDIV2)
		this->rot.x = XM_PIDIV2;
	if (this->rot.z < -XM_PIDIV2)
		this->rot.z = -XM_PIDIV2;
	else if (this->rot.z > XM_PIDIV2)
		this->rot.z = XM_PIDIV2;
	if (this->rot.y < 0)
		this->rot.y += XM_2PI;
	else if (this->rot.y > XM_2PI)
		this->rot.y -= XM_2PI;
	UpdateViewMatrix();
}

XMVECTOR Camera::GetPosition()
{
	return this->pos;
}

XMFLOAT3 Camera::GetRotation()
{
	return this->rot;
}

const XMMATRIX Camera::GetViewMatrix()
{
	return this->viewMatrix;
}

const XMMATRIX Camera::GetProjectionMatrix()
{
	return this->projectionMatrix;
}

void Camera::SetProjectionValues(float FOV, float width, float height, float nearZ, float farZ)
{
	this->projectionMatrix = XMMatrixPerspectiveFovLH((FOV / 180)*3.14f, (float)width / height, nearZ, farZ); //90 degree field of view
}

const XMVECTOR & Camera::GetForwardVector()
{
	this->vec_forward.m128_f32[1] = 0; //set y part of vector to 0
	return this->vec_forward;
}

const XMVECTOR & Camera::GetRightVector()
{
	this->vec_right.m128_f32[1] = 0; //set y part of vector to 0
	return this->vec_right;
}

const XMVECTOR & Camera::GetBackwardVector()
{
	this->vec_backward.m128_f32[1] = 0; //set y part of vector to 0
	return this->vec_backward;
}

const XMVECTOR & Camera::GetLeftVector()
{
	this->vec_left.m128_f32[1] = 0; //set y part of vector to 0
	return this->vec_left;
}

void Camera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
{
	//Calculate camera rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);
	//Adjust cam target to be offset by the camera's current position
	camTarget += this->pos;
	//Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);

	this->viewMatrix = XMMatrixLookAtLH(this->pos, camTarget, upDir);

	XMMATRIX camRotationMatrix2 = XMMatrixRotationRollPitchYaw(0, this->rot.y, this->rot.z);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix2);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, camRotationMatrix2);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, camRotationMatrix2);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, camRotationMatrix2);
}
