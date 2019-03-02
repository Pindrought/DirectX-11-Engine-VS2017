#include "AdapterReader.h"

std::vector<AdapterData> AdapterReader::adapters;

std::vector<AdapterData> AdapterReader::GetAdapters()
{
	if (adapters.size() > 0) //If already initialized
		return adapters;

	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

	// Create a DXGIFactory object.
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"Failed to create DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	IDXGIAdapter * pAdapter;
	UINT index = 0;
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
	{
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	return adapters;
}

AdapterData::AdapterData(IDXGIAdapter * pAdapter)
{
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&description);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, L"Failed to Get Description for IDXGIAdapter.");
	}
}
