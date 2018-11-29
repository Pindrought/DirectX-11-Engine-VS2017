#ifndef MESH_H
#define MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

#include <vector>
#include <d3d11_1.h>
#include "Vertex.h"
using namespace DirectX;

struct Texture 
{
	string type;
	string path;
	ID3D11ShaderResourceView *texture;
};

class Mesh 
{
public:
	vector<Vertex> vertices;
	vector<UINT> indices;
	vector<Texture> textures;
	ID3D11Device *dev;

	Mesh(ID3D11Device *dev, vector<Vertex> vertices, vector<UINT> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->dev = dev;

		this->setupMesh(dev);
	}

	void Draw(ID3D11DeviceContext *devcon)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		devcon->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
		devcon->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		devcon->PSSetShaderResources(0, 1, &textures[0].texture);
		devcon->DrawIndexed(indices.size(), 0, 0);
	}

private:
	/*  Render data  */
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	bool setupMesh(ID3D11Device *dev)
	{
		HRESULT hr;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertices[0];

		hr = dev->CreateBuffer(&vbd, &initData, VertexBuffer.GetAddressOf());
		if (FAILED(hr))
			return false;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * indices.size();
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;

		initData.pSysMem = &indices[0];

		hr = dev->CreateBuffer(&ibd, &initData, IndexBuffer.GetAddressOf());
		if (FAILED(hr))
			return false;
	}
};

#endif