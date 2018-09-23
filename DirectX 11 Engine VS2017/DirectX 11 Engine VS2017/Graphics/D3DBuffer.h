#ifndef D3DBUFFER_H
#define D3DBUFFER_H

#include <D3D11.h>
template <typename T>
class D3DBuffer
{
private:
	ID3D11Buffer * buffer;
public:
	T data;    // buffer data


	D3DBuffer() {}
	~D3DBuffer() { }


	ID3D11Buffer* Buffer()const;

	HRESULT Initialize(ID3D11Device * dev, unsigned int Usage = D3D11_USAGE_DYNAMIC, unsigned int BindFlags = D3D11_BIND_CONSTANT_BUFFER, unsigned int CPUAccessFlags = D3D11_CPU_ACCESS_WRITE);

	HRESULT Update(ID3D11DeviceContext * devCon);

};
#endif