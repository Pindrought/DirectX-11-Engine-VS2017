#include "Model.h"

bool Model::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * texture)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;



	return true;
}

void Model::Draw()
{
	UINT offset = 0;
	this->deviceContext->PSSetShaderResources(0, 1, &texture);
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexBuffer.BufferSize(), 0, 0);
}
