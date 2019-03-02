#ifndef ConstantBuffer_h__
#define ConstantBuffer_h__
#include "ConstantBufferTypes.h"
#include "..\\ErrorLogger.h"
#include "PipelineManager.h"

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);

private:

	ComPtr<ID3D11Buffer> buffer = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;

public:
	ConstantBuffer() {}

	T data;

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	HRESULT Initialize()
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();

		deviceContext = PipelineManager::GetDeviceContextPtr();

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		desc.StructureByteStride = 0;

		ID3D11Device * device = PipelineManager::GetDevicePtr();
		HRESULT hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());
		return hr;
	}

	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, L"Failed to map constant buffer.");
			return false;
		}
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		deviceContext->Unmap(buffer.Get(), 0);
		return true;
	}
};

#endif // ConstantBuffer_h__