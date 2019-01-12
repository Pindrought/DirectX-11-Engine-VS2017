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
	float dynamicLightStrength; 
	DirectX::XMFLOAT3 dynamicLightPosition; 
	float dynamicLightAttenuation_a;
	float dynamicLightAttenuation_b;
	float dynamicLightAttenuation_c;
};