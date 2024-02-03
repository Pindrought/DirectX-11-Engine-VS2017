//Camera Matrices Demo [WIP] 2018-10-05
#include "Engine.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	int width = GetSystemMetrics(SM_CXFULLSCREEN) * 0.9;
	int height = GetSystemMetrics(SM_CYFULLSCREEN) * 0.9;

	Engine engine;
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", width, height))
	{
		while (engine.ProcessMessages() == true)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}
	return 0;
}