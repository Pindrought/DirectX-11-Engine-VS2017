#pragma once
#include "GameObject2D.h"

class Camera2D : public GameObject2D
{
public:
	Camera2D();
	void SetProjectionValues(float width, float height, float nearZ, float farZ);

	const XMMATRIX & GetOrthoMatrix() const;
	const XMMATRIX & GetWorldMatrix() const;
private:
	void UpdateMatrix() override;

	XMMATRIX orthoMatrix;
	XMMATRIX worldMatrix;
};
