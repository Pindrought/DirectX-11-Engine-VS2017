#pragma once
#include "ErrorLogger.h"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"DirectXTK.lib")

class RenderWindow
{
public:
	bool Initialize(HINSTANCE hInstance, int width = 800, int height = 600, std::string window_title = "Title", std::string window_class = "renderwinclass");
	bool ProcessMessages();
	~RenderWindow();
private:
	void RegisterWindowClass();
	HWND handle = NULL; //Handle to this window
	HINSTANCE hInstance = NULL; //Handle to application instance
	std::string window_title = "";
	std::wstring window_title_wide = L""; //Wide string representation of window title
	std::string window_class = "";
	std::wstring window_class_wide = L""; //Wide string representation of window class name
	int width = 0;
	int height = 0;
};