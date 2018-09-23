//Tutorial 13 Solution 2018-09-23
#include "Engine.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Engine engine;
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600))
	{
		while (engine.ProcessMessages() == true)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}
	return 0;
}

//INPUT ASSEMBLER - COMPLETED
//VERTEX SHADER - COMPLETED
//RASTERIZER - COMPLETED
//PIXEL SHADER - 
//OUTPUT MERGER - COMPLETED