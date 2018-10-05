#include "Graphics.h"
#include <sstream>
#include <iomanip>

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

struct MatrixProjection
{
	enum MatrixType
	{
		RotationRollPitchYaw,
		Translation,
		Scale
	};

	MatrixProjection(MatrixType mtype)
		:type(mtype)
	{
		switch (mtype)
		{
		case MatrixType::Scale:
		{
			values[0] = 1;
			values[1] = 1;
			values[2] = 1;
			break;
		}
		}
	}
	MatrixType type;
	float values[3] = { 0,0,0 };
};

void Graphics::RenderFrame()
{

	Camera renderCam = camera;


	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	this->deviceContext->IASetInputLayout(this->vs_3d_textures.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vs_3d_textures.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(ps_3d_textures.GetShader(), NULL, 0);
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//this->deviceContext->OMSetBlendState(0, 0, 0xffffffff); //set blend state to opaque for solid objects

	UINT offset = 0;
	static float rotRoll(0), rotPitch(0), rotYaw(0);
	static std::vector<MatrixProjection> matrices;
	
	cb_vs_vertexshader.data.wvp = XMMatrixIdentity();

	for (int i = 0; i < matrices.size(); i++)
	{
		switch (matrices[i].type)
		{
		case MatrixProjection::Scale:
			cb_vs_vertexshader.data.wvp = cb_vs_vertexshader.data.wvp * XMMatrixScaling(matrices[i].values[0], matrices[i].values[1], matrices[i].values[2]);
			break;
		case MatrixProjection::RotationRollPitchYaw:
			cb_vs_vertexshader.data.wvp = cb_vs_vertexshader.data.wvp * XMMatrixRotationRollPitchYaw(matrices[i].values[0], matrices[i].values[1], matrices[i].values[2]);
			break;
		case MatrixProjection::Translation:
			cb_vs_vertexshader.data.wvp = cb_vs_vertexshader.data.wvp * XMMatrixTranslation(matrices[i].values[0], matrices[i].values[1], matrices[i].values[2]);
			break;
		}
	}

	XMMATRIX finalMat = cb_vs_vertexshader.data.wvp;
	static float eyePos[3] = { 0.0f, 0.0f, -2.0f };
	static float lookatPos[3] = { 0.0f, 0.0f, 1.0f };

	if (eyePos[0] == lookatPos[0] && eyePos[1] == lookatPos[1] && eyePos[2] == lookatPos[2])
	{
		eyePos[2] -= 1.0f;
	}

	static float upVector[3] = { 0.0f, 1.0f, 0.0f };
	if (upVector[0] == 0 && upVector[1] == 0 && upVector[2] == 0)
	{
		upVector[1] = 0.1f;
	}
	XMMATRIX camTranslationOffset = XMMatrixTranslation(-eyePos[0], -eyePos[1], -eyePos[2]);
	XMVECTOR eyePosVec = { eyePos[0], eyePos[1], eyePos[2], 0.0f };
	XMVECTOR lookatVec = { lookatPos[0], lookatPos[1], lookatPos[2], 1.0f };
	XMVECTOR upVectorVec = { upVector[0], upVector[1], upVector[2], 1.0f };
	XMMATRIX camViewMatrix = XMMatrixLookAtLH(eyePosVec, lookatVec, upVectorVec);

	static float FOV = 90.0f;
	float fovRadians = FOV / 180 * 3.14;
	float aspectRatio = (float)windowWidth / (float)windowHeight;
	static float nearZ = 1.0f;
	static float farZ = 10.0f;

	if (farZ < nearZ)
		farZ = nearZ + 0.1f;
	if (farZ < 0.2f)
		farZ = 0.2f;
	if (nearZ < 0.1f)
		nearZ = 0.1f;
	if (nearZ > farZ)
		nearZ = farZ - 0.1f;


	Camera lensCam;
	lensCam.SetProjectionValues(FOV, windowWidth, windowHeight, nearZ, farZ);
	lensCam.SetViewMatrix(camViewMatrix);
	if (seeThroughCameraLens)
	{
		renderCam = lensCam;
	}

	if (seeThroughCameraLens)
		cb_vs_vertexshader.data.wvp = finalMat * renderCam.GetViewMatrix() * renderCam.GetProjectionMatrix();
	else
		cb_vs_vertexshader.data.wvp = finalMat * camViewMatrix * renderCam.GetViewMatrix() * renderCam.GetProjectionMatrix();
	cb_vs_vertexshader.data.wvp = XMMatrixTranspose(cb_vs_vertexshader.data.wvp);
	cb_vs_vertexshader.ApplyChanges();


	this->deviceContext->VSSetShader(vs_3d_textures.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(ps_3d_textures.GetShader(), NULL, 0);
	this->deviceContext->IASetInputLayout(this->vs_3d_textures.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, this->vb_texture.GetAddressOf(), this->vb_texture.Stride(), &offset);
	this->deviceContext->IASetIndexBuffer(this->ib_texture.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());
	this->deviceContext->DrawIndexed(this->ib_texture.BufferSize(), 0, 0);


	if (seeThroughCameraLens == false)
	{
		XMMATRIX camMatrix = XMMatrixRotationRollPitchYaw(-XM_PIDIV2, 0, 0) * XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.25f, 0.2f, -1.0f);
		this->camera_model->Draw(this->deviceContext.Get(), *states, camMatrix, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}

	this->deviceContext->IASetInputLayout(this->vs_3d_colors.GetInputLayout());
	this->deviceContext->VSSetShader(vs_3d_colors.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(ps_3d_colors.GetShader(), NULL, 0);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	if (seeThroughCameraLens)
		cb_vs_vertexshader.data.wvp = renderCam.GetViewMatrix() * renderCam.GetProjectionMatrix();
	else
		cb_vs_vertexshader.data.wvp = camViewMatrix * renderCam.GetViewMatrix() * renderCam.GetProjectionMatrix();
	cb_vs_vertexshader.data.wvp = XMMatrixTranspose(cb_vs_vertexshader.data.wvp);
	cb_vs_vertexshader.ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());


	UINT stride2 = sizeof(Vertex_COLOR);
	this->deviceContext->IASetVertexBuffers(0, 1, vb_grid.GetAddressOf(), vb_grid.Stride(), &offset);
	
	static bool renderXYaxis = true;
	static bool renderXZaxis = true;
	
	if (renderXZaxis)
		this->deviceContext->Draw(this->vb_grid.BufferSize()/2, 0);
	if (renderXYaxis)
		this->deviceContext->Draw(this->vb_grid.BufferSize() / 2, this->vb_grid.BufferSize() / 2);

	static bool drawFrustum = true;
	static bool wireframeFrustum = false;
	if (seeThroughCameraLens == false && drawFrustum == true)
	{
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		float blendFactor[] = { 0.5f, 0.5f, 0.5f, 0.5f };
		this->deviceContext->OMSetBlendState(this->transparentBlendState.Get(), blendFactor, 0xffffffff);
		if (wireframeFrustum)
			this->deviceContext->RSSetState(this->rasterizerState_noCull_wireFrame.Get());
		else
			this->deviceContext->RSSetState(this->rasterizerState_noCull.Get());

		float nearHeight = 2 * tan(fovRadians / 2) * nearZ;
		float farHeight = 2 * tan(fovRadians / 2) * farZ;
		float nearWidth = nearHeight * aspectRatio;
		float farWidth = farHeight * aspectRatio;
		std::vector<Vertex_COLOR> data;
		float alpha = 0.5f;
		data.push_back(Vertex_COLOR((-nearWidth * 0.5), (nearHeight*0.5), nearZ, 0.6f, 0.0f, 0.0f, alpha)); //Near Topleft
		data.push_back(Vertex_COLOR((nearWidth*0.5), (nearHeight*0.5), nearZ, 0.6f, 0.0f, 0.0f, alpha)); //Near Topright
		data.push_back(Vertex_COLOR((nearWidth*0.5), (-nearHeight * 0.5), nearZ, 0.6f, 0.0f, 0.0f, alpha)); //Near Bottomright
		data.push_back(Vertex_COLOR(-(nearWidth*0.5), (-nearHeight * 0.5), nearZ, 0.6f, 0.0f, 0.0f, alpha)); //Near Bottomleft
		data.push_back(Vertex_COLOR((-farWidth * 0.5), (farHeight*0.5), farZ, 0.0f, 0.9f, 0.0f, alpha)); //Far Topleft
		data.push_back(Vertex_COLOR((farWidth*0.5), (farHeight*0.5), farZ, 0.0f, 0.9f, 0.0f, alpha)); //Far Topright
		data.push_back(Vertex_COLOR((farWidth*0.5), (-farHeight * 0.5), farZ, 0.0f, 0.9f, 0.0f, alpha)); //Far Bottomright
		data.push_back(Vertex_COLOR(-(farWidth*0.5), (-farHeight * 0.5), farZ, 0.0f, 0.9f, 0.0f, alpha)); //Far Bottomleft

		DWORD indices[] =
		{
			//Near Plane
			0, 1, 2, 
			0, 2, 3,
			//Far Plane
			4, 5, 6,
			4, 6, 7,
			//Left Plane
			0, 4, 7,
			0, 7, 3,
			//Right Plane
			1, 5, 6,
			1, 6, 2,
			//Top Plane
			0, 1, 4,
			1, 4, 5,
			//Bottom Plane
			2, 3, 6,
			3, 6, 7,
		};

		this->vb_frustum.Initialize(this->device.Get(), data.data(), data.size());
		this->ib_frustum.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
		cb_vs_vertexshader.data.wvp = renderCam.GetViewMatrix() * renderCam.GetProjectionMatrix();
		cb_vs_vertexshader.data.wvp = XMMatrixTranspose(cb_vs_vertexshader.data.wvp);
		cb_vs_vertexshader.ApplyChanges();
		this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());
		this->deviceContext->IASetVertexBuffers(0, 1, this->vb_frustum.GetAddressOf(), this->vb_frustum.Stride(), &offset);
		this->deviceContext->IASetIndexBuffer(ib_frustum.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->DrawIndexed(ARRAYSIZE(indices), 0, 0);
		this->deviceContext->RSSetState(this->rasterizerState.Get());
		this->deviceContext->OMSetBlendState(0, 0, 0xffffffff); //set blend state to opaque for solid objects

	}

	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static bool detailed_matrix_info = false;
		static bool display_final_matrix = false;
		static int counter = 0;
		static float clear_color[3];


		ImGui::Begin("Camera Window");

		ImGui::Text("View Matrix Fields.");               // Display some text (you can use a format strings too)
		
		ImGui::Checkbox("Draw Camera Viewing Frustum", &drawFrustum);
		ImGui::SameLine();
		ImGui::Checkbox("Wireframe", &wireframeFrustum);
		ImGui::Checkbox("Use Virtual Camera Lens", &seeThroughCameraLens);
		ImGui::Text("View Matrix Settings");
		ImGui::SameLine();
		if (ImGui::Button("Reset View Matrix"))
		{
			eyePos[0] = 0.0f;
			eyePos[1] = 0.0f;
			eyePos[2] = -2.0f;
			lookatPos[0] = 0.0f;
			lookatPos[1] = 0.0f;
			lookatPos[2] = 1.0f;
			upVector[0] = 0.0f;
			upVector[1] = 1.0f;
			upVector[2] = 0.0f;
		}
		ImGui::DragFloat3("Eye Pos X/Y/Z", eyePos, 0.1f, -25.0f, 25.0f);
		ImGui::DragFloat3("Lookat Pos X/Y/Z", lookatPos, 0.1f, -25.0f, 25.0f);
		ImGui::DragFloat3("Up Vector X/Y/Z", upVector, 0.1f, -25.0f, 25.0f);
		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::Text("Projection Matrix Settings.");
		ImGui::SameLine();
		if (ImGui::Button("Reset Projection Matrix"))
		{
			FOV = 90.0f;
			nearZ = 1.0f;
			farZ = 10.0f;
		}
		ImGui::Text("Aspect Ratio = Width/Height [Locked]");
		ImGui::DragFloat("FOV (Degrees)", &FOV, 0.1f, 0.1f, 180.0f);
		ImGui::DragFloat("Near Z (Must be Less than Far Z)", &nearZ, 0.1f, 0.0f, farZ - 0.1f);
		ImGui::DragFloat("Far Z (Must be Greater than Near Z)", &farZ, 0.1f, nearZ + 0.1f, 50.0f);

		ImGui::End();


		ImGui::Begin("Movable Object Window");                      

		ImGui::Text("Matrix Visual Demo by Jpres.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Render X-Y Axis", &renderXYaxis);
		ImGui::Checkbox("Render X-Z Axis", &renderXZaxis);


		if (ImGui::Button("Reset Matrices"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			for (int i = 0; i < matrices.size(); i++)
			{
				if (matrices[i].type == MatrixProjection::Scale)
				{
					matrices[i].values[0] = 1.0f;
					matrices[i].values[1] = 1.0f;
					matrices[i].values[2] = 1.0f;
				}
				else
				{
					matrices[i].values[0] = 0.0f;
					matrices[i].values[1] = 0.0f;
					matrices[i].values[2] = 0.0f;
				}
			}
		}
		ImGui::SameLine();


		if (ImGui::Button("Clear Matrices"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			matrices.clear();

		ImGui::SameLine();
		if (ImGui::Button("Add Scaling Matrix"))                            
			matrices.push_back(MatrixProjection(MatrixProjection::Scale));
		ImGui::SameLine();

		if (ImGui::Button("Add Rotation Matrix"))
			matrices.push_back(MatrixProjection(MatrixProjection::RotationRollPitchYaw));
		ImGui::SameLine();

		if (ImGui::Button("Add Translation Matrix"))
			matrices.push_back(MatrixProjection(MatrixProjection::Translation));

		
		ImGui::Checkbox("Detailed Matrix Info", &detailed_matrix_info);
		if (detailed_matrix_info)
			ImGui::Checkbox("Display Final Matrix", &display_final_matrix);


		ImGui::NewLine();
		ImGui::NewLine();
		for (int i = 0; i < matrices.size(); i++)
		{
			switch (matrices[i].type)
			{
			case MatrixProjection::Scale:
			{
				std::string name = "Scale X/Y/Z [" + std::to_string(i) + "]";
				ImGui::DragFloat3(name.c_str(), matrices[i].values, 0.1f, 0.0f, 5.0f);
				ImGui::SameLine();
				std::string removename = "Remove [" + std::to_string(i) + "]";
				if (ImGui::Button(removename.c_str()))
				{
					matrices.erase(matrices.begin() + i);
					i -= 1;
					continue;
				}
				break;
			}
			case MatrixProjection::RotationRollPitchYaw:
			{
				std::string name = "Rotation Pitch/Yaw/Roll [" + std::to_string(i) + "]";
				ImGui::DragFloat3(name.c_str(), matrices[i].values, 0.1f, -XM_2PI, XM_2PI);
				ImGui::SameLine();
				std::string removename = "Remove [" + std::to_string(i) + "]";
				if (ImGui::Button(removename.c_str()))
				{
					matrices.erase(matrices.begin() + i);
					i -= 1;
					continue;
				}
				break;
			}
			case MatrixProjection::Translation:
			{
				std::string name = "Translation X/Y/Z [" + std::to_string(i) + "]";
				ImGui::DragFloat3(name.c_str(), matrices[i].values, 0.1f, -20.0f, 20.0f);
				ImGui::SameLine();
				std::string removename = "Remove [" + std::to_string(i) + "]";
				if (ImGui::Button(removename.c_str()))
				{
					matrices.erase(matrices.begin() + i);
					i -= 1;
					continue;
				}
				break;
			}
			}
			if (detailed_matrix_info)
			{
				using namespace std;
				DirectX::XMMATRIX mat;
				switch (matrices[i].type)
				{
				case MatrixProjection::Scale:
					mat = XMMatrixScaling(matrices[i].values[0], matrices[i].values[1], matrices[i].values[2]);
					break;
				case MatrixProjection::RotationRollPitchYaw:
					mat = XMMatrixRotationRollPitchYaw(matrices[i].values[0], matrices[i].values[1], matrices[i].values[2]);
					break;
				case MatrixProjection::Translation:
					mat = XMMatrixTranslation(matrices[i].values[0], matrices[i].values[1], matrices[i].values[2]);
					break;
				}
				DisplayMatrix(mat);
			}
		}

		if (display_final_matrix && detailed_matrix_info)
		{
			ImGui::Text("Final Matrix");
			DisplayMatrix(finalMat);
		}

		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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
	rasterizerDesc.DepthClipEnable = TRUE;

	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;

	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState_noCull.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;

	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState_noCull_wireFrame.GetAddressOf());
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

	//create transparent blend state for frustum drawing
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = 0x0f;

	//blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	hr = this->device->CreateBlendState(&blendDesc, this->transparentBlendState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create blend state.");
		return false;
	}


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

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
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
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
		//Front
		Vertex(-0.5f,  -0.5f, 0.0f, 0.0f, 1.0f), //Bottom Left   - [0]
		Vertex(-0.5f,   0.5f, 0.0f, 0.0f, 0.0f), //Top Left      - [1]
		Vertex( 0.5f,   0.5f, 0.0f, 0.5f, 0.0f), //Top Right     - [2]
		Vertex(0.5f,  -0.5f, 0.0f, 0.5f, 1.0f), //Bottom Right   - [3]
		//Back
		Vertex(0.5f,  -0.5f, 0.0f, 0.5f, 1.0f), //Bottom Left   - [4]
		Vertex(0.5f,   0.5f, 0.0f, 0.5f, 0.0f), //Top Left      - [5]
		Vertex(-0.5f,   0.5f, 0.0f, 1.0f, 0.0f), //Top Right     - [6]
		Vertex(-0.5f,  -0.5f, 0.0f, 1.0f, 1.0f), //Bottom Right   - [7]
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
		0, 2, 3,
		4, 5, 6,
		4, 6, 7
	};

	hr = this->ib_texture.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer for textured square.");
		return hr;
	}
	

	//Load Texture
	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\pianofrontback.png", nullptr, myTexture.GetAddressOf());
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
	float alpha = 1.0f;
	//Draw -Z -> +Z (Red->White) //[X-Z Axis]
	for (int i = 0; i < gridSections; i++)
	{
		v2.push_back(Vertex_COLOR(0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, -gridDimension / 2.0f, 1.0f, 0.0f, 0.0f, alpha));
		v2.push_back(Vertex_COLOR(0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, +(gridDimension) / 2.0f - gridDimension / gridSections, 1.0f, 1.0f, 1.0f, alpha));
	}
	//Draw -X -> +X (Blue->White)  //[X-Z Axis]
	for (int i = 0; i < gridSections; i++)
	{
		v2.push_back(Vertex_COLOR(-gridDimension/2.0f, 0.0f, 0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, 0.0f, 1.0f, alpha));
		v2.push_back(Vertex_COLOR(+gridDimension/2.0f - gridDimension / gridSections, 0.0f, 0.0f - gridDimension / 2 + i * gridDimension / gridSections, 1.0f, 1.0f, 1.0f, alpha));
	}
	//Draw -X -> +X (Blue->White) //[X-Y Axis]
	for (int i = 0; i < gridSections; i++)
	{
		v2.push_back(Vertex_COLOR(-gridDimension / 2.0f, 0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, 0.0f, 0.0f, 1.0f, alpha));
		v2.push_back(Vertex_COLOR(+gridDimension / 2.0f - gridDimension / gridSections, 0.0f - gridDimension / 2 + i * gridDimension / gridSections, 0.0f, 1.0f, 1.0f, 1.0f, alpha));
	}
	//Draw -Y -> +Y (Green->White) //[X-Y Axis]
	for (int i = 0; i < gridSections; i++)
	{
		v2.push_back(Vertex_COLOR(0.0f - gridDimension / 2 + i * gridDimension / gridSections, -gridDimension/2.0f, 0.0f, 0.0f, 1.0f, 0.0f, alpha));
		v2.push_back(Vertex_COLOR(0.0f - gridDimension / 2 + i * gridDimension / gridSections, +gridDimension / 2.0f - gridDimension / gridSections, 0.0f, 1.0f, 1.0f, 1.0f, alpha));
	}

	hr = vb_grid.Initialize(this->device.Get(), v2.data(), v2.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}


	camera.SetProjectionValues(90, windowWidth, windowHeight, 0.1f, 1000.0f);
	camera.SetPosition(0.0f, 0.0f, 0.0f);
	ImGui::SetNextWindowSize(ImVec2(1000, 400));


	states = std::make_unique<CommonStates>(this->device.Get());

	effectFactory = std::make_unique<EffectFactory>(this->device.Get());

	camera_model = DirectX::Model::CreateFromSDKMESH(this->device.Get(), L"Data\\Meshes\\camera.sdkmesh", *effectFactory, true);

	return true;
}

void Graphics::DisplayMatrix(const XMMATRIX mat)
{
	for (int j = 0; j < 4; j++)
	{
		std::stringstream ss;
		std::streamsize width = 10;
		ss.precision(3);
		ss << "[" << std::setw(width) << mat.r[j].m128_f32[0] << "," << std::setw(width) << mat.r[j].m128_f32[1] << "," << std::setw(width) << mat.r[j].m128_f32[2] << "," << std::setw(width) << mat.r[j].m128_f32[3] << "]";
		ImGui::Text(ss.str().c_str());
	}
}
