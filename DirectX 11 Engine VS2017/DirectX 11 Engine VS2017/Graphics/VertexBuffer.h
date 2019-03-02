#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "PipelineManager.h"

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride = sizeof(T);
	UINT vertexCount = 0;

public:
	VertexBuffer() {}


	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT VertexCount() const
	{
		return vertexCount;
	}

	const UINT Stride() const
	{
		return stride;
	}

	const UINT * StridePtr() const
	{
		return &stride;
	}

	HRESULT Initialize(T * data, UINT vertexCount)
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();

		this->vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		ID3D11Device * device = PipelineManager::GetDevicePtr();
		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, buffer.GetAddressOf());
		return hr;
	}
};

#endif // VertexBuffer_h__
