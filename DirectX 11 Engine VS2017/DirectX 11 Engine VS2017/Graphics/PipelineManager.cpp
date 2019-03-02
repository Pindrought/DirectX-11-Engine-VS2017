#include "PipelineManager.h"


PipelineManager::PipelineManager()
{
}

#pragma region
//SharedPtr/Comptr RegisterResource Definitions
#define PIPELINE_RESOURCE_SHAREDPTR_OBJECT(datatype, mapname) \
void PipelineManager::RegisterResource(std::wstring id, std::shared_ptr<datatype> resource) \
{ \
	resourceMaps.mapname[id] = resource; \
}
#define PIPELINE_RESOURCE_COMPTR_OBJECT(datatype, mapname) \
void PipelineManager::RegisterResource(std::wstring id, ComPtr<datatype> resource) \
{ \
	resourceMaps.mapname[id] = resource; \
}
#include "PipelineManager.inl"

//SharedPtr/Comptr GetResource Definitions
#define PIPELINE_RESOURCE_SHAREDPTR_OBJECT(datatype, mapname) \
bool PipelineManager::GetResource(std::wstring id, datatype *& pOutResource) \
{ \
	std::unordered_map<std::wstring, std::shared_ptr<datatype>>::iterator it; \
	it = resourceMaps.mapname.find(id); \
 \
	if (it != resourceMaps.mapname.end()) /*If resource was found*/ \
	{ \
		pOutResource = it->second.get(); \
		return true; /*return true if resource was found*/ \
	} \
\
	pOutResource = nullptr; \
	return false; /*return false if no matching resource found*/ \
}
#define PIPELINE_RESOURCE_COMPTR_OBJECT(datatype, mapname) \
bool PipelineManager::GetResource(std::wstring id, datatype *& pOutResource) \
{ \
	std::unordered_map<std::wstring, ComPtr<datatype>>::iterator it; \
	it = resourceMaps.mapname.find(id); \
 \
	if (it != resourceMaps.mapname.end()) /*If resource was found*/ \
	{ \
		pOutResource = it->second.Get(); \
		return true; /*return true if resource was found*/ \
	} \
\
	pOutResource = nullptr; \
	return false; /*return false if no matching resource found*/ \
}
#include "PipelineManager.inl"
#pragma endregion This is a region to generate the definitions for the GetResource and RegisterResource functions for the data types from the PipelineManager.inl inline file

//bool PipelineManager::GetResource(std::wstring id, VertexShader *& pOutResource)
//{
//	std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>::iterator it;
//	it = resourceMaps.vertexshaders.find(id);
//	
//	if (it != resourceMaps.vertexshaders.end()) //If resource was found
//	{
//		pOutResource = it->second.get();
//		return true; //return true if resource was found
//	}
//
//	pOutResource = nullptr;
//	return false; //return false if no matching resource found
//}





//IPipelineResource * PipelineManager::GetResource(PipelineResourceType type, std::wstring name)
//{
//	std::unordered_map<std::wstring, IPipelineResource*>::iterator it;
//	std::unordered_map<std::wstring, IPipelineResource*> * usemap = nullptr;
//	switch (type)
//	{
//	case PipelineResourceType::PRT_PixelShader:
//		usemap = &resourceMaps.pixelshaders;
//		break;
//	case PipelineResourceType::PRT_VertexShader:
//		usemap = &resourceMaps.vertexshaders;
//		break;
//	default:
//		break;
//	}
//
//	it = usemap->find(name);
//
//	if (it != usemap->end()) //If resource was found
//		return it->second; //return ptr to resource retrieved from map
//
//	return nullptr; //return nullptr if resource was not found
//}

void PipelineManager::RegisterDevice(ComPtr<ID3D11Device> device)
{
	PipelineManager::device = device;
}

void PipelineManager::RegisterDeviceContext(ComPtr<ID3D11DeviceContext> deviceContext)
{
	PipelineManager::deviceContext = deviceContext;
}

ID3D11Device * PipelineManager::GetDevicePtr()
{
	return PipelineManager::device.Get();
}

ID3D11DeviceContext * PipelineManager::GetDeviceContextPtr()
{
	return PipelineManager::deviceContext.Get();
}

void PipelineManager::NewFrame()
{
	pipelineAttributes.inputLayout = nullptr;
	pipelineAttributes.depthStencilState = nullptr;
	pipelineAttributes.stencilRef = 0;
	pipelineAttributes.blendState = nullptr;
	pipelineAttributes.blendFactor = nullptr;
	pipelineAttributes.blendSampleMask = 0xFFFFFFFF;
	pipelineAttributes.rasterizerState = nullptr;
	pipelineAttributes.pixelshader = nullptr;
	pipelineAttributes.vertexshader = nullptr;
	pipelineAttributes.topology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

}

void PipelineManager::SetInputLayout(ID3D11InputLayout * inputLayout)
{
	if (pipelineAttributes.inputLayout != inputLayout)
	{
		pipelineAttributes.inputLayout = inputLayout;
		deviceContext->IASetInputLayout(inputLayout);
	}
}

void PipelineManager::SetDepthStencilState(ID3D11DepthStencilState * depthStencilState, UINT stencilRef)
{
	if (pipelineAttributes.depthStencilState != depthStencilState || pipelineAttributes.stencilRef != stencilRef)
	{
		pipelineAttributes.depthStencilState = depthStencilState;
		pipelineAttributes.stencilRef = stencilRef;

		deviceContext->OMSetDepthStencilState(depthStencilState, stencilRef);
	}
}

void PipelineManager::SetBlendState(ID3D11BlendState * blendState, FLOAT * blendFactor, UINT sampleMask)
{
	if (pipelineAttributes.blendState != blendState || pipelineAttributes.blendFactor != blendFactor || pipelineAttributes.blendSampleMask != sampleMask)
	{
		pipelineAttributes.blendState = blendState;
		pipelineAttributes.blendFactor = blendFactor;
		pipelineAttributes.blendSampleMask = sampleMask;

		deviceContext->OMSetBlendState(blendState, blendFactor, sampleMask);
	}
}

void PipelineManager::SetPixelShader(ID3D11PixelShader * pixelshader)
{
	if (pixelshader != pipelineAttributes.pixelshader)
	{
		pipelineAttributes.pixelshader = pixelshader;
		deviceContext->PSSetShader(pixelshader, NULL, 0);
	}
}

void PipelineManager::SetVertexShader(ID3D11VertexShader * vertexshader)
{
	if (vertexshader != pipelineAttributes.vertexshader)
	{
		pipelineAttributes.vertexshader = vertexshader;
		deviceContext->VSSetShader(vertexshader, NULL, 0);
	}
}

void PipelineManager::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	if (pipelineAttributes.topology != topology)
	{
		pipelineAttributes.topology = topology;
		deviceContext->IASetPrimitiveTopology(topology);
	}
}

void PipelineManager::SetRasterizerState(ID3D11RasterizerState * rasterizerState)
{
	if (pipelineAttributes.rasterizerState != rasterizerState)
	{
		pipelineAttributes.rasterizerState = rasterizerState;
		deviceContext->RSSetState(rasterizerState);
	}
}


PipelineResourceMaps PipelineManager::resourceMaps;
ComPtr<ID3D11Device> PipelineManager::device;
ComPtr<ID3D11DeviceContext> PipelineManager::deviceContext;
PipelineActiveAttributes PipelineManager::pipelineAttributes;

PipelineManager pm; //Initialize an instance of a resource manager to call static constructor

