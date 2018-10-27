#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

template<class T>
class InstanceBuffer
{
private:
	InstanceBuffer(const InstanceBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize = 0;
	UINT instanceCount = 0;
	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
public:
	InstanceBuffer() {}

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT BufferSize() const
	{
		return this->bufferSize;
	}

	UINT InstanceCount() const
	{
		return this->instanceCount;
	}
	void Initialize(ID3D11Device *device, ID3D11DeviceContext * deviceContext)
	{
		if (buffer != nullptr)
			buffer.Reset();
		this->device = device;
		this->deviceContext = deviceContext;
	}

	bool UpdateInstanceData(T * data, UINT instanceCount)
	{
		this->instanceCount = 0;
		if (bufferSize < (instanceCount * sizeof(T)))
		{
			buffer.Reset();
			bufferSize = 0;
			CD3D11_BUFFER_DESC instanceBufferDesc(sizeof(T) * instanceCount, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

			D3D11_SUBRESOURCE_DATA instData = {};

			instData.pSysMem = data;
			HRESULT hr = this->device->CreateBuffer(&instanceBufferDesc, &instData, buffer.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				bufferSize = sizeof(T) * instanceCount;
				this->instanceCount = instanceCount;
			}
			return SUCCEEDED(hr);
		}
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = this->deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to map instance buffer.");
			return false;
		}
		CopyMemory(mappedResource.pData, data, bufferSize);
		this->deviceContext->Unmap(buffer.Get(), 0);
		this->instanceCount = instanceCount;
		return true;
	}
};