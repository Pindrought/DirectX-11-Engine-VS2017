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
	Vertex_COLOR(float x, float y, float z, float r, float g, float b, float a)
		: pos(x, y, z, 1.0f), color(r, g, b, a) {}

	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 color;
};