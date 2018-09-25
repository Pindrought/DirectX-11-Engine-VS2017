#ifndef ConstantBuffer_h__
#define ConstantBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

struct CB_VS
{
	DirectX::XMMATRIX  rotationMat;
};

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	bool mInitialized;

public:
	ConstantBuffer() {}

	T Data;

	ID3D11Buffer* Buffer()const
	{
		return buffer.Get();
	}

	ID3D11Buffer *const * BufferAddress()const
	{
		return buffer.GetAddressOf();
	}

	HRESULT Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device)
	{
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		desc.StructureByteStride = 0;

		hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());
		mInitialized = true;
		return hr;
	}

	void ApplyChanges(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &dc)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = dc->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &Data, sizeof(T));
		dc->Unmap(buffer.Get(), 0);
	}
};

#endif // ConstantBuffer_h__