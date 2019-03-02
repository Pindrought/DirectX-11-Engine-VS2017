#pragma once
#include <unordered_map>
#include <wrl/client.h>
#include <d3d11.h>
#include "Shaders.h"

using namespace Microsoft::WRL;

struct PipelineActiveAttributes //Active pixel shader, vertex shader, etc
{
	ID3D11InputLayout * inputLayout = nullptr;
	ID3D11DepthStencilState * depthStencilState = nullptr;
	UINT stencilRef = 0;
	ID3D11BlendState * blendState = nullptr;
	FLOAT * blendFactor = nullptr;
	UINT blendSampleMask = 0xFFFFFFFF;
	ID3D11RasterizerState * rasterizerState = nullptr;
	ID3D11PixelShader * pixelshader = nullptr;
	ID3D11VertexShader * vertexshader = nullptr;
	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

struct PipelineResourceMaps
{
	//Maps that use shared ptr
#define PIPELINE_RESOURCE_SHAREDPTR_OBJECT(datatype, mapname) \
	std::unordered_map<std::wstring, std::shared_ptr<datatype>> mapname; 

	//Maps that use comptr
#define PIPELINE_RESOURCE_COMPTR_OBJECT(datatype, mapname) \
	std::unordered_map<std::wstring, ComPtr<datatype>> mapname;

	//Include inline to generate the shared_ptr and comptr maps
#include "PipelineManager.inl"
};

class PipelineManager
{
public:
	PipelineManager();

#pragma region
	//Shared ptr/comptr register resource declarations
#define PIPELINE_RESOURCE_SHAREDPTR_OBJECT(datatype, mapname) \
	static void RegisterResource(std::wstring id, std::shared_ptr<datatype> resource);
#define PIPELINE_RESOURCE_COMPTR_OBJECT(datatype, mapname) \
	static void RegisterResource(std::wstring id, ComPtr<datatype> resource);
#include "PipelineManager.inl"

	//Shared ptr/comptr get resource declarations
#define PIPELINE_RESOURCE_SHAREDPTR_OBJECT(datatype, mapname) \
	static bool GetResource(std::wstring id, datatype * &pOutResource);
#define PIPELINE_RESOURCE_COMPTR_OBJECT(datatype, mapname) \
	static bool GetResource(std::wstring id, datatype * &pOutResource);
#include "PipelineManager.inl"
#pragma endregion This region generates the declarations for the comptr/sharedptr map object functions GetResource and RegisterResource

	static void RegisterDevice(ComPtr<ID3D11Device> device);
	static void RegisterDeviceContext(ComPtr<ID3D11DeviceContext> deviceContext);
	static ID3D11Device * GetDevicePtr();
	static ID3D11DeviceContext * GetDeviceContextPtr();

	static void NewFrame();
	static void SetInputLayout(ID3D11InputLayout * inputLayout);
	static void SetDepthStencilState(ID3D11DepthStencilState * depthStencilState, UINT stencilRef = 0);
	static void SetBlendState(ID3D11BlendState * blendState, FLOAT * blendFactor = nullptr, UINT sampleMask = 0xFFFFFFFF);

	static void SetPixelShader(ID3D11PixelShader * pixelshader);
	static void SetVertexShader(ID3D11VertexShader * vertexshader);
	static void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
	static void SetRasterizerState(ID3D11RasterizerState * rasterizerState);

private:
	static PipelineResourceMaps resourceMaps;
	static ComPtr<ID3D11Device> device;
	static ComPtr<ID3D11DeviceContext> deviceContext;
	static PipelineActiveAttributes pipelineAttributes;
};