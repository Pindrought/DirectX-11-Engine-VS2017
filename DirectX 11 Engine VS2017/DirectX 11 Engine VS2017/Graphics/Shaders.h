#pragma once
#include "..\\ErrorLogger.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

using namespace Microsoft::WRL;
class VertexShader
{
public:
	bool Initialize(std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements);
	ID3D11VertexShader * GetShader();
	ID3D10Blob * GetBuffer();
	ID3D11InputLayout * GetInputLayout();
private:
	ComPtr<ID3D11VertexShader> shader;
	ComPtr<ID3D10Blob> shader_buffer;
	ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader
{
public:
	bool Initialize(std::wstring shaderpath);
	ID3D11PixelShader * GetShader();
	ID3D10Blob * GetBuffer();
private:
	ComPtr<ID3D11PixelShader> shader;
	ComPtr<ID3D10Blob> shader_buffer;
};

