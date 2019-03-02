#pragma once
#include "RenderableGameObject3D.h"

class Light : public RenderableGameObject3D
{
public:
	bool Initialize(ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
};