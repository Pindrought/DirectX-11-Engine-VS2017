#pragma once
#include <DirectXMath.h>

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z, float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};

struct Vertex_COLOR
{
	Vertex_COLOR() {}
	Vertex_COLOR(float x, float y, float z, float r, float g, float b)
		: pos(x, y, z), color(r, g, b) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};