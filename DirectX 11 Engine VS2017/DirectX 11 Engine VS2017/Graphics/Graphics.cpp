#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	if (!InitializeDirectX(hwnd, width, height))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetInputLayout(this->vs_3d_textures.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vs_3d_textures.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(ps_3d_textures.GetShader(), NULL, 0);

	UINT offset = 0;

	

	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, this->vb_texture.GetAddressOf(), this->vb_texture.Stride(), &offset);
	this->deviceContext->IASetIndexBuffer(this->ib_texture.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());
	this->deviceContext->DrawIndexed(this->ib_texture.BufferSize(), 0, 0);

	this->deviceContext->IASetInputLayout(this->vs_3d_colors.GetInputLayout());
	this->deviceContext->VSSetShader(vs_3d_colors.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(ps_3d_colors.GetShader(), NULL, 0);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	cb_vs_vertexshader.data.wvp = XMMatrixTranslation(0, 0, 1) * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	cb_vs_vertexshader.data.wvp = XMMatrixTranspose(cb_vs_vertexshader.data.wvp);
	cb_vs_vertexshader.ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());


	UINT stride2 = sizeof(Vertex_COLOR);
	this->deviceContext->IASetVertexBuffers(0, 1, vb_grid.GetAddressOf(), vb_grid.Stride(), &offset);
	this->deviceContext->Draw(this->vb_grid.BufferSize(), 0);

	this->swapchain->Present(1, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd, int width, int height)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No IDXGI Adapters found.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(	adapters[0].pAdapter, //IDXGI Adapter
										D3D_DRIVER_TYPE_UNKNOWN,
										NULL, //FOR SOFTWARE DRIVER TYPE
										NULL, //FLAGS FOR RUNTIME LAYERS
										NULL, //FEATURE LEVELS ARRAY
										0, //# OF FEATURE LEVELS IN ARRAY
										D3D11_SDK_VERSION,
										&scd, //Swapchain description
										this->swapchain.GetAddressOf(), //Swapchain Address
										this->device.GetAddressOf(), //Device Address
										NULL, //Supported feature level
										this->deviceContext.GetAddressOf()); //Device Context Address

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create device and swapchain.");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create render target view.");
		return false;
	}

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view.");
		return false;
	}

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state.");
		return false;
	}

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	//Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

	//Create sampler description for sampler state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create sampler state.");
		return false;
	}

	return true;
}

bool Graphics::InitializeShaders()
{

	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG //Debug Mode
	#ifdef _WIN64 //x64
			shaderfolder = L"..\\x64\\Debug\\";
	#else  //x86 (Win32)
			shaderfolder = L"..\\Debug\\";
	#endif
	#else //Release Mode
	#ifdef _WIN64 //x64
			shaderfolder = L"..\\x64\\Release\\";
	#else  //x86 (Win32)
			shaderfolder = L"..\\Release\\";
	#endif
#endif
	}
	UINT numElements;
	D3D11_INPUT_ELEMENT_DESC inputLayout_3d_textures[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	numElements = ARRAYSIZE(inputLayout_3d_textures);

	if (!vs_3d_textures.Initialize(this->device, shaderfolder + L"vs_3d_textures.cso", inputLayout_3d_textures, numElements))
		return false;

	if (!ps_3d_textures.Initialize(this->device, shaderfolder + L"ps_3d_textures.cso"))
		return false;

	D3D11_INPUT_ELEMENT_DESC inputLayout_3d_colors[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	numElements = ARRAYSIZE(inputLayout_3d_colors);


	if (!vs_3d_colors.Initialize(this->device, shaderfolder + L"vs_3d_colors.cso", inputLayout_3d_colors, numElements))
		return false;

	if (!ps_3d_colors.Initialize(this->device, shaderfolder + L"ps_3d_colors.cso"))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	//Textured Square
	Vertex v[] =
	{
		Vertex(-0.5f,  -0.5f, 0.0f, 0.0f, 1.0f), //Bottom Left   - [0]
		Vertex(-0.5f,   0.5f, 0.0f, 0.0f, 0.0f), //Top Left      - [1]
		Vertex( 0.5f,   0.5f, 0.0f, 1.0f, 0.0f), //Top Right     - [2]
		Vertex(0.5f,  -0.5f, 0.0f, 1.0f, 1.0f), //Bottom Right   - [3]
	};

	HRESULT hr = this->vb_texture.Initialize(this->device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer for textured square.");
		return false;
	}

	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	hr = this->ib_texture.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer for textured square.");
		return hr;
	}
	

	//Load Texture
	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\piano.png", nullptr, myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	//Set up constant buffer for vertex shader
	hr = cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create constant buffer.");
		return false;
	}

	//draw x grid
	std::vector<Vertex_COLOR> v2;
	float gridDimension = 25.0f;
	int gridSections = 25;
	//Draw -Z -> +Z (Red->White)
	for (int i = 0; i < gridSections; i++)
	{
		v2.push_back(Vertex_COLOR(0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, -gridDimension / 2.0f, 1.0f, 0.0f, 0.0f));
		v2.push_back(Vertex_COLOR(0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, +(gridDimension) / 2.0f - gridDimension / gridSections, 1.0f, 1.0f, 1.0f));
	}
	//Draw -X -> +X (Blue->White)
	for (int i = 0; i < gridSections; i++)
	{
		v2.push_back(Vertex_COLOR(-gridDimension/2.0f, 0.0f, 0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, 0.0f, 1.0f));
		v2.push_back(Vertex_COLOR(+gridDimension/2.0f - gridDimension / gridSections, 0.0f, 0.0f - gridDimension / 2 + i * gridDimension / gridSections, 1.0f, 1.0f, 1.0f));
		v2.push_back(Vertex_COLOR(-gridDimension / 2.0f, 0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, 0.0f, 0.0f, 1.0f));
		v2.push_back(Vertex_COLOR(+gridDimension / 2.0f - gridDimension / gridSections, 0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, 1.0f, 1.0f, 1.0f));
	}
	//Draw -Y -> +Y (Green->White)
	for (int i = 0; i < gridSections; i++)
	{
		v2.push_back(Vertex_COLOR(0.0f - gridDimension / 2 + i * gridDimension / gridSections, -gridDimension/2.0f, 0.0f, 0.0f, 1.0f, 0.0f));
		v2.push_back(Vertex_COLOR(0.0f - gridDimension / 2 + i * gridDimension / gridSections, +gridDimension / 2.0f - gridDimension / gridSections, 0.0f, 1.0f, 1.0f, 1.0f));
	}

	hr = vb_grid.Initialize(this->device.Get(), v2.data(), v2.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}


	camera.SetProjectionValues(90, windowWidth, windowHeight, 1.0f, 1000.0f);
	camera.SetPosition(0.0f, 0.0f, -1.0f);

	return true;
}
