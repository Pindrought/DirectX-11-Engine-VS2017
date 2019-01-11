#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct DynamicLightData
{
	DirectX::XMFLOAT3 dynamicLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float dynamicLightStrength = 1.0f;
	DirectX::XMFLOAT3 dynamicLightPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float padding2;
};

struct CB_PS_light
{
	DirectX::XMFLOAT3 ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float ambientLightStrength = 0.5f;

	float dynamicLightCount = 0;
	DirectX::XMFLOAT3 padding;

	DynamicLightData dynamicLights[10];
};