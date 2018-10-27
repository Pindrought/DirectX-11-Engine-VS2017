#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame()
{

	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);
	
	{ 
		this->model.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}

	//Draw Text
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringConverter::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f,0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	static int counter = 0;
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGui Test Window
	ImGui::Begin("Test");
	ImGui::End();
	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(0, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No IDXGI Adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		scd.BufferDesc.Width = this->windowWidth;
		scd.BufferDesc.Height = this->windowHeight;
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
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
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

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		//Describe our Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = this->device->CreateTexture2D(&depthStencilTextureDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//Create & set the Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));;
		this->deviceContext->RSSetViewports(1, &viewport);

		//Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Rasterizer State for culling front
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerState_CullFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Blend State
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC blendDesc = { 0 };
		blendDesc.RenderTarget[0] = rtbd;

		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
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

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexshader.Initialize(this->device, shaderfolder + L"vertexshader.cso", layout, numElements))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshader.cso"))
		return false;


	return true;
}

bool Graphics::InitializeScene()
{
	try
	{
		//Load Texture
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//Initialize Constant Buffer(s)
		hr = this->cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		hr = this->cb_ps_pixelshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		if (!model.Initialize(this->device.Get(), this->deviceContext.Get(), this->pavementTexture.Get(), this->cb_vs_vertexshader))
			return false;

		camera.SetPosition(0.0f, 0.0f, -2.0f);
		camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}
