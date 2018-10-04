#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "ConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <Model.h>
#include <Effects.h>
#include <CommonStates.h>



class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	Camera camera;

private:
	bool InitializeDirectX(HWND hwnd, int width, int height);
	bool InitializeShaders();
	bool InitializeScene();

	void DisplayMatrix(const XMMATRIX mat);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexShader vs_3d_textures;
	PixelShader ps_3d_textures;
	VertexShader vs_3d_colors;
	PixelShader ps_3d_colors;

	VertexBuffer<Vertex> vb_texture;
	IndexBuffer ib_texture;
	VertexBuffer<Vertex_COLOR> vb_grid;
	VertexBuffer<Vertex_COLOR> vb_frustum;
	IndexBuffer ib_frustum;

	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_noCull;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;

	std::unique_ptr<DirectX::CommonStates> states;
	std::unique_ptr<DirectX::EffectFactory> effectFactory;
	std::unique_ptr<DirectX::Model> camera_model;

	int windowWidth = 0;
	int windowHeight = 0;
};