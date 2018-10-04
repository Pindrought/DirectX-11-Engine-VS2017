////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

class Camera
{
public:
	Camera();

	void SetPosition(float x, float y, float z);
	void AdjustPosition(XMVECTOR posOffset);
	void SetRotation(float xRot, float yRot, float zRot);
	void AdjustRotation(float xRotOffset, float yRotOffset, float zRotOffset);

	XMVECTOR GetPosition();
	XMFLOAT3 GetRotation();

	std::vector<XMFLOAT4> GetFrustumPlanes();

	const XMMATRIX GetViewMatrix();
	const XMMATRIX GetProjectionMatrix();
	void SetProjectionValues(float FOV, float width, float height, float nearZ, float farZ);
	void SetViewMatrix(XMMATRIX mat);
	const XMVECTOR & GetForwardVector();
	const XMVECTOR & GetRightVector();
	const XMVECTOR & GetBackwardVector();
	const XMVECTOR & GetLeftVector();

	Camera& operator=(const Camera& camera); // copy assignment
	

private:
	void UpdateViewMatrix();
	XMVECTOR pos;
	XMFLOAT3 rot;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = { 0, 0, 1 };
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = { 0, 0, -1 };
	const XMVECTOR DEFAULT_UP_VECTOR = { 0, 1, 0 };
	const XMVECTOR DEFAULT_LEFT_VECTOR = { -1, 0, 0 };
	const XMVECTOR DEFAULT_RIGHT_VECTOR = { 1, 0, 0 };

	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
};

#endif