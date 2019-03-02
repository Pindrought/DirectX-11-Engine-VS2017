#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex3D>& vertices, std::vector<DWORD>& indices, std::vector<Texture> & textures, const DirectX::XMMATRIX & transformMatrix)
{
	deviceContext = PipelineManager::GetDeviceContextPtr();
	this->textures = textures;
	this->transformMatrix = transformMatrix;

	HRESULT hr = vertexbuffer.Initialize(vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, L"Failed to initialize vertex buffer for mesh.");

	hr = indexbuffer.Initialize(indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, L"Failed to initialize index buffer for mesh.");
}

Mesh::Mesh(const Mesh & mesh)
{
	deviceContext = mesh.deviceContext;
	indexbuffer = mesh.indexbuffer;
	vertexbuffer = mesh.vertexbuffer;
	textures = mesh.textures;
	transformMatrix = mesh.transformMatrix;
}

void Mesh::Draw()
{
	UINT offset = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
		{
			deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
	}

	deviceContext->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), vertexbuffer.StridePtr(), &offset);
	deviceContext->IASetIndexBuffer(indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indexbuffer.IndexCount(), 0, 0);
}

const DirectX::XMMATRIX & Mesh::GetTransformMatrix()
{
	return transformMatrix;
}