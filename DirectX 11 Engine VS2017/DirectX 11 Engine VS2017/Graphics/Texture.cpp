#include "Texture.h"
#include "..\\ErrorLogger.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

Texture::Texture(ID3D11Device * device, const Color & color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device * device, const Color * colorData, UINT width, UINT height, aiTextureType type)
{
	this->InitializeColorTexture(device, colorData, width, height, type);
}

Texture::Texture(ID3D11Device * device, const std::string & filePath, aiTextureType type)
{
	this->type = type;
	if (StringHelper::GetFileExtension(filePath) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
		return;
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
		return;
	}
}

Texture::Texture(ID3D11Device * device, const uint8_t * pData, size_t size, aiTextureType type)
{
	this->type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(device, pData, size, this->texture.GetAddressOf(), this->textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create Texture from memory.");
}

aiTextureType Texture::GetType()
{
	return this->type;
}

ID3D11ShaderResourceView * Texture::GetTextureResourceView()
{
	return this->textureView.Get();
}

ID3D11ShaderResourceView ** Texture::GetTextureResourceViewAddress()
{
	return this->textureView.GetAddressOf();
}

void Texture::Initialize1x1ColorTexture(ID3D11Device * device, const Color & colorData, aiTextureType type)
{
	InitializeColorTexture(device, &colorData, 1, 1, type);
}

void Texture::InitializeColorTexture(ID3D11Device * device, const Color * colorData, UINT width, UINT height, aiTextureType type)
{
	this->type = type;

	const int textureWidth = 16;
	const int textureHeight = 16;

	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, textureWidth, textureHeight, 1, 1, D3D11_BIND_SHADER_RESOURCE);
	ID3D11Texture2D * p2DTexture = nullptr;

	Color data[textureHeight][textureWidth];
	for (int i = 0; i < textureHeight; i++)
	{
		for (int j = 0; j < textureWidth; j++)
		{
			data[i][j].SetR(120 * (i*j % 2));
			data[i][j].SetG(0);
			data[i][j].SetB(0);

			data[i][j].SetA(255);
		}
	}

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = data;
	initialData.SysMemPitch = textureWidth * sizeof(Color);

	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data.");
	texture = static_cast<ID3D11Texture2D*>(p2DTexture);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data.");
}