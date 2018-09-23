#include "D3DBuffer.h"

template<typename T>
ID3D11Buffer * D3DBuffer<T>::Buffer() const
{
	return buffer;
}

template<typename T>
HRESULT D3DBuffer<T>::Initialize(ID3D11Device * dev, unsigned int Usage, unsigned int BindFlags, unsigned int CPUAccessFlags)
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd; // buffer description
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = Usage;
	bd.BindFlags = BindFlags;
	bd.CPUAccessFlags = CPUAccessFlags;
	bd.ByteWidth = sizeof(data);    // size of buffer
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	hr = dev->CreateBuffer(&bd, NULL, &buffer);   // create the buffer
	return hr;
}

template<typename T>
HRESULT D3DBuffer<T>::Update(ID3D11DeviceContext * devCon)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE ms;
	hr = devCon->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, data, sizeof(data));                                        // copy the data
	devCon->Unmap(buffer, NULL);                                        // unmap the buffer
	return hr;
}
//
//template<typename T>
//D3DBuffer<T>::~D3DBuffer()
//{
//}