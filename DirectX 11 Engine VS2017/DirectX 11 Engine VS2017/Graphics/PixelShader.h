#pragma once
#include "..\\ErrorLogger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class PixelShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath);
	ID3D11PixelShader * GetShader();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer = nullptr;
};