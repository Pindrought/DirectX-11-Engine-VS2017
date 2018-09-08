#pragma once
#include "RenderWindow.h"

class Engine
{
public:
	bool Initialize(HINSTANCE hInstance, int width = 800, int height = 600, std::string window_title = "Title", std::string window_class = "renderwinclass");
	bool ProcessMessages();
private:
	HINSTANCE hInstance = NULL;
	RenderWindow render_window;
};