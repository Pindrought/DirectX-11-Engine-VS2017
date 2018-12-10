#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"

class Mesh
{
public:
	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext, std::vector<Vertex> & vertices, std::vector<DWORD> & indices, std::vector<Texture> & textures);
	Mesh(const Mesh & mesh);
	void Draw();
private:
	VertexBuffer<Vertex> vertexbuffer;
	IndexBuffer indexbuffer;
	ID3D11DeviceContext * deviceContext;
	std::vector<Texture> textures;
};