#include "Graphics.h"
#include <DirectXMath.h>

struct Vertex    //Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z)
		: pos(x, y, z) {}


	DirectX::XMFLOAT3 pos;
};

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	if (!InitializeDirectX(hwnd, width, height))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	matrices.push_back(DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	matrices.push_back(DirectX::XMMatrixTranslation(0.1f, 0.0f, 0.0f));
	matrices.push_back(DirectX::XMMatrixTranslation(-0.1f, 0.0f, 0.0f));
	matrices.push_back(DirectX::XMMatrixTranslation(0.0f, +0.1f, 0.0f));
	matrices.push_back(DirectX::XMMatrixTranslation(0.0f, -0.2f, 0.0f));

	D3D11_BUFFER_DESC instBuffDesc;
	ZeroMemory(&instBuffDesc, sizeof(instBuffDesc));

	instBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	instBuffDesc.ByteWidth = sizeof(DirectX::XMMATRIX) * matrices.size();
	instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instBuffDesc.CPUAccessFlags = 0;
	instBuffDesc.MiscFlags = 0;
	instBuffDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA instData;
	ZeroMemory(&instData, sizeof(instData));

	instData.pSysMem = matrices.data();
	HRESULT hr = this->device->CreateBuffer(&instBuffDesc, &instData, instanceBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "createbuffer");
	}


	return true;
}

void Graphics::RenderFrame()
{
	ID3D11Buffer* vertInstBuffers[2] = { vertBuffer.Get() , instanceBuffer.Get() };

	UINT strides[2] = { sizeof(Vertex), sizeof(DirectX::XMMATRIX) };
	UINT offsets[2] = { 0, 0 };
	this->deviceContext->IASetVertexBuffers(0, 2, vertInstBuffers, strides, offsets);


	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	//this->deviceContext->Draw(1, 0);


	this->deviceContext->DrawInstanced(1, matrices.size(), 0, 0);


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
										2, //FLAGS FOR RUNTIME LAYERS
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

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), NULL);

	return true;
}

bool Graphics::InitializeShaders()
{
	std::wstring shaderfolder;
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
	else
	{
		shaderfolder = L"";
	}
#pragma endregion Determines the Shader path depending on Debug/Release/x86/x64/Visual Studio Environment or Standalone

	if (!vertexshader.Initialize(this->device, shaderfolder + L"vertexshader.cso"))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshader.cso"))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEMAT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEMAT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEMAT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEMAT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	UINT numElements = ARRAYSIZE(layout);


	


	HRESULT hr = this->device->CreateInputLayout(layout, numElements, vertexshader.GetBuffer()->GetBufferPointer(),
		vertexshader.GetBuffer()->GetBufferSize(), inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create input layout.");
		return false;
	}

	Vertex v[] =
	{
		Vertex(0.0f, 0.0f, 0.0f),
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	hr = this->device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertBuffer);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}


	//Set the Input Layout
	this->deviceContext->IASetInputLayout(inputLayout.Get());

	//Set Primitive Topology
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;
	viewport.Height = 600;

	//Set the Viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	return true;
}
