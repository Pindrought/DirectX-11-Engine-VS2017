#pragma once
#include "ErrorLogger.h"

class WindowContainer;

class RenderWindow
{
public:
	bool Initialize(WindowContainer * pWindowContainer, HINSTANCE hInstance, std::wstring window_title, std::wstring window_class, int width, int height);
	bool ProcessMessages();
	HWND GetHWND() const;
	~RenderWindow();
private:
	void RegisterWindowClass();
	HWND handle = NULL; //Handle to this window
	HINSTANCE hInstance = NULL; //Handle to application instance
	std::wstring window_title = L"";
	std::wstring window_class = L"";
	int width = 0;
	int height = 0;
};