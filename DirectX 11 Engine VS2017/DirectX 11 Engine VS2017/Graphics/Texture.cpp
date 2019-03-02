#include "Texture.h"
#include "..\\ErrorLogger.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "PipelineManager.h"

Texture::Texture(const Color & color, aiTextureType type)
{
	Initialize1x1ColorTexture(color, type);
}

Texture::Texture(const Color * colorData, UINT width, UINT height, aiTextureType type)
{
	InitializeColorTexture(colorData, width, height, type);
}

Texture::Texture(const std::wstring & filePath, aiTextureType type)
{
	this->type = type;
	ID3D11Device * device = PipelineManager::GetDevicePtr();

	if (StringHelper::GetFileExtension(filePath) == L".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, filePath.c_str(), texture.GetAddressOf(), textureView.GetAddressOf());
		if (FAILED(hr))
		{
			Initialize1x1ColorTexture(Colors::UnloadedTextureColor, type);
		}
		return;
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, filePath.c_str(), texture.GetAddressOf(), textureView.GetAddressOf());
		if (FAILED(hr))
		{
			Initialize1x1ColorTexture(Colors::UnloadedTextureColor, type);
		}
		return;
	}
}

Texture::Texture(const uint8_t * pData, size_t size, aiTextureType type)
{
	this->type = type;
	ID3D11Device * device = PipelineManager::GetDevicePtr();
	HRESULT hr = DirectX::CreateWICTextureFromMemory(device, pData, size, texture.GetAddressOf(), textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, L"Failed to create Texture from memory.");
}

aiTextureType Texture::GetType()
{
	return type;
}

ID3D11ShaderResourceView * Texture::GetTextureResourceView()
{
	return textureView.Get();
}

ID3D11ShaderResourceView ** Texture::GetTextureResourceViewAddress()
{
	return textureView.GetAddressOf();
}

void Texture::Initialize1x1ColorTexture(const Color & colorData, aiTextureType type)
{
	InitializeColorTexture(&colorData, 1, 1, type);
}

void Texture::InitializeColorTexture(const Color * colorData, UINT width, UINT height, aiTextureType type)
{
	this->type = type;
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

	ID3D11Texture2D * p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = width * sizeof(Color);

	ID3D11Device * device = PipelineManager::GetDevicePtr();
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, L"Failed to initialize texture from color data.");
	texture = static_cast<ID3D11Texture2D*>(p2DTexture);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, L"Failed to create shader resource view from texture generated from color data.");
}