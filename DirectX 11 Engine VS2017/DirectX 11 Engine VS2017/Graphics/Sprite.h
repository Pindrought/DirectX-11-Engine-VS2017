#pragma once
#include "GameObject2D.h"
#include <string>

class Sprite : GameObject2D
{
public:
	bool Initialize(int width, int height, std::string spritePath);
	void Draw(XMMATRIX orthoMatrix); //2d camera orthogonal matrix
private:

};