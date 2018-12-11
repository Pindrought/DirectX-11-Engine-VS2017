#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_PS_light
{
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;
	DirectX::XMFLOAT3 directionalLightColor;
	float directionalLightStrength;
	DirectX::XMFLOAT3 directionalLightDirection;
	float padding;
	DirectX::XMFLOAT3 viewPosition;
};