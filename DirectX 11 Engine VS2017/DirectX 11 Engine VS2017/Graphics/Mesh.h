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
	Mesh(std::vector<Vertex3D> & vertices, std::vector<DWORD> & indices, std::vector<Texture> & textures, const XMMATRIX & transformMatrix);
	Mesh(const Mesh & mesh);
	void Draw();
	const XMMATRIX & GetTransformMatrix();
private:
	VertexBuffer<Vertex3D> vertexbuffer;
	IndexBuffer indexbuffer;
	ID3D11DeviceContext * deviceContext = nullptr;
	std::vector<Texture> textures;
	XMMATRIX transformMatrix;
};