#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "Camera2D.h"
#include "Camera3D.h"

#include "..\\Timer.h"
#include "ImGUI\\imgui.h"
#include "ImGUI\\imgui_impl_win32.h"
#include "ImGUI\\imgui_impl_dx11.h"
#include "RenderableGameObject3D.h"
#include "Light.h"
#include "Sprite.h"

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	Camera3D camera3D;
	Camera2D camera2D;
	std::vector<RenderableGameObject3D> gameObjects;
	Light light;
	Sprite sprite;
private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	std::shared_ptr<VertexShader> vertexshader = std::make_shared<VertexShader>();
	std::shared_ptr<VertexShader> vertexshader_2d = std::make_shared<VertexShader>();

	std::shared_ptr<PixelShader> pixelshader = std::make_shared<PixelShader>();
	std::shared_ptr<PixelShader> pixelshader_nolight = std::make_shared<PixelShader>();
	std::shared_ptr<PixelShader> pixelshader_2d = std::make_shared<PixelShader>();
	std::shared_ptr<PixelShader> pixelshader_2d_mask = std::make_shared<PixelShader>();


	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;

	ConstantBuffer<CB_PS_light> cb_ps_light;


	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_maskpredraw;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_maskpostdraw;


	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStateTransparency;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

	int windowWidth = 0;
	int windowHeight = 0;
	Timer fpsTimer;
};