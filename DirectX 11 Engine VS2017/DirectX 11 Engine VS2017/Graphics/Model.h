#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <DirectXMath.h>

using namespace DirectX;

class Model
{
public:
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * texture);
	void Draw();
	void SetPosition(float x, float y, float z);
private:
	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ID3D11ShaderResourceView * texture = nullptr;
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;
	XMFLOAT3 position;
	XMVECTOR positionVector;
};