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
	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightDiffuseStrength;
	DirectX::XMFLOAT3 dynamicLightPosition;
	float dynamicLightSpecularStrength;
	float lightAttenuationConstantFactor;
	float lightAttenuationLinearFactor;
	float lightAttenuationExponentialFactor;
	float padding;
	DirectX::XMFLOAT3 viewPosition;
};