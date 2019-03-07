#include "Graphics.h"
#include "PipelineManager.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	fpsTimer.Start();

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
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame()
{
	cb_ps_light.data.dynamicLightColor = light.lightColor;
	cb_ps_light.data.dynamicLightStrength = light.lightStrength;
	cb_ps_light.data.dynamicLightPosition = light.GetPositionFloat3();
	cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
	cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
	cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;
	cb_ps_light.ApplyChanges();
	deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());

	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	PipelineManager::NewFrame();
	PipelineManager::SetDepthStencilState(depthStencilState.Get());
	PipelineManager::SetBlendState(nullptr);
	PipelineManager::SetRasterizerState(rasterizerState.Get());
	PipelineManager::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	PipelineManager::SetBlendState(nullptr);

	//draw 2d stuff
	//PipelineManager::SetBlendState(this->blendState.Get(), nullptr, 0x000000FF);

	PipelineManager::SetDepthStencilState(depthStencilState_maskpredraw.Get());

	PipelineManager::SetInputLayout(vertexshader_2d->GetInputLayout());
	PipelineManager::SetPixelShader(pixelshader_2d_mask->GetShader());

	PipelineManager::SetVertexShader(vertexshader_2d->GetShader());
	sprite.Draw(camera2D.GetOrthoMatrix());

	//draw 3d stuff

	PipelineManager::SetDepthStencilState(depthStencilState_maskpostdraw.Get(), 0);

	PipelineManager::SetInputLayout(vertexshader->GetInputLayout());
	PipelineManager::SetPixelShader(pixelshader->GetShader());
	PipelineManager::SetVertexShader(vertexshader->GetShader());
	{
		for (auto & object : gameObjects)
		{
			object.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
		}
	}
	{
		PipelineManager::SetPixelShader(pixelshader_nolight->GetShader());
		light.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
	}

	

	//Draw Text
	static int fpsCounter = 0;
	static std::wstring fpsString = L"FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = L"FPS: " + std::to_wstring(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), fpsString.c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f,0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	static int counter = 0;
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGui Test Window
	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &light.lightColor.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Strength", &light.lightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation A", &light.attenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation B", &light.attenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Attenuation C", &light.attenuation_c, 0.01f, 0.0f, 10.0f);
	ImGui::End();
	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swapchain->Present(0, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log(L"No IDXGI Adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		scd.BufferDesc.Width = windowWidth;
		scd.BufferDesc.Height = windowHeight;
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
			swapchain.GetAddressOf(), //Swapchain Address
			device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			deviceContext.GetAddressOf()); //Device Context Address

		COM_ERROR_IF_FAILED(hr, L"Failed to create device and swapchain.");

		PipelineManager::RegisterDevice(device);
		PipelineManager::RegisterDeviceContext(deviceContext);


		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, L"GetBuffer Failed.");

		hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create render target view.");

		//Describe our Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = device->CreateTexture2D(&depthStencilTextureDesc, NULL, depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create depth stencil buffer.");

		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create depth stencil view.");

		deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create depth stencil state.");

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc_maskpredraw(D3D11_DEFAULT);
		depthstencildesc_maskpredraw.DepthEnable = FALSE;
		depthstencildesc_maskpredraw.StencilEnable = TRUE;
		//never do backface shit
		depthstencildesc_maskpredraw.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		//always do front face
		depthstencildesc_maskpredraw.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		depthstencildesc_maskpredraw.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR_SAT;
		depthstencildesc_maskpredraw.StencilWriteMask = 0xFF;
		depthstencildesc_maskpredraw.StencilReadMask = 0xFF;

		hr = device->CreateDepthStencilState(&depthstencildesc_maskpredraw, depthStencilState_maskpredraw.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create depth stencil state for mask predraw.");

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc_maskpostdraw(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		depthstencildesc_maskpostdraw.StencilEnable = TRUE;
		//never do backface shit
		depthstencildesc_maskpostdraw.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		//always do front face
		depthstencildesc_maskpostdraw.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		depthstencildesc_maskpostdraw.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_maskpostdraw.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthstencildesc_maskpostdraw.StencilWriteMask = 0xFF;
		depthstencildesc_maskpostdraw.StencilReadMask = 0xFF;

		hr = device->CreateDepthStencilState(&depthstencildesc_maskpostdraw, depthStencilState_maskpostdraw.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create depth stencil state for mask POSTDRAW.");

		//Create & set the Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));;
		deviceContext->RSSetViewports(1, &viewport);

		//Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		rasterizerDesc.CullMode = D3D11_CULL_NONE;

		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create rasterizer state.");

		//Create Rasterizer State for culling front
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = device->CreateRasterizerState(&rasterizerDesc_CullFront, rasterizerState_CullFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create rasterizer state.");

		//Create Blend State
		/*D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;*/

		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALPHA;



		D3D11_BLEND_DESC blendDesc = { 0 };
		blendDesc.RenderTarget[0] = rtbd;

		hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, L"Failed to create blend state.");

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, L"Failed to create sampler state.");
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

	D3D11_INPUT_ELEMENT_DESC layout_3d[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	D3D11_INPUT_ELEMENT_DESC layout_2d[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElements = ARRAYSIZE(layout_3d);
	
	if (!vertexshader->Initialize(shaderfolder + L"vertexshader.cso", layout_3d, numElements))
		return false;
	
	PipelineManager::RegisterResource(L"vs_3d", vertexshader);

	numElements = ARRAYSIZE(layout_2d);
	if (!vertexshader_2d->Initialize(shaderfolder + L"vertexshader_2d.cso", layout_2d, numElements))
		return false;

	PipelineManager::RegisterResource(L"vs_2d", vertexshader);


	if (!pixelshader->Initialize(shaderfolder + L"pixelshader.cso"))
		return false;

	PipelineManager::RegisterResource(L"ps_3d", pixelshader);

	if (!pixelshader_nolight->Initialize(shaderfolder + L"pixelshader_nolight.cso"))
		return false;

	PipelineManager::RegisterResource(L"ps_3d_nolight", pixelshader_nolight);
	
	if (!pixelshader_2d->Initialize(shaderfolder + L"pixelshader_2d.cso"))
		return false;

	PipelineManager::RegisterResource(L"ps_2d", pixelshader_2d);

	if (!pixelshader_2d_mask->Initialize(shaderfolder + L"pixelshader_2d_mask.cso"))
		return false;

	PipelineManager::RegisterResource(L"ps_2d_mask", pixelshader_2d_mask);

	return true;
}

bool Graphics::InitializeScene()
{
	try
	{
		//Initialize Constant Buffer(s)
		HRESULT hr = cb_vs_vertexshader.Initialize();
		COM_ERROR_IF_FAILED(hr, L"Failed to initialize constant buffer.");

		hr = cb_vs_vertexshader_2d.Initialize();
		COM_ERROR_IF_FAILED(hr, L"Failed to initialize 2d constant buffer.");

		hr = cb_ps_light.Initialize();
		COM_ERROR_IF_FAILED(hr, L"Failed to initialize constant buffer.");

		cb_ps_light.data.ambientLightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
		cb_ps_light.data.ambientLightStrength = 1.0f;

		gameObjects.resize(1);
		float xPos = 0.0f;
		for (auto & object : gameObjects)
		{
			/*if (!object.Initialize("Data\\Objects\\Nanosuit\\Nanosuit.obj", cb_vs_vertexshader))
				return false;*/
			if (!object.Initialize("Data/Objects/nanosuit/nanosuit.obj", cb_vs_vertexshader))
				return false;
			object.SetPosition(xPos, 0.0f, 0.0f);
			xPos += 5.0f;
		}

		if (!light.Initialize(cb_vs_vertexshader))
			return false;

		camera3D.SetPosition(0.0f, 0.0f, -2.0f);
		camera3D.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);

		camera2D.SetProjectionValues(windowWidth, windowHeight, 0, 1.0f);
		camera2D.SetPosition(0.0f, 0.0f, 0.0f);

		if (!sprite.Initialize(700, 700, L"Data/Textures/star.png", cb_vs_vertexshader_2d))
			return false;

		sprite.SetPosition(windowWidth/2.0f - sprite.GetWidth()/2.0f, windowHeight/2.0f - sprite.GetHeight()/2.0f, 0.0f);
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}
