#pragma once
#include <DirectXMath.h>

struct Vertex    //Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y)
		: pos(x, y) {}

	DirectX::XMFLOAT2 pos;
};