#pragma once
#include "RenderWindow.h"
#include "Keyboard/KeyboardClass.h"
#include "Mouse/MouseClass.h"
#include "Graphics\Graphics.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow render_window;
	KeyboardClass keyboard;
	MouseClass mouse;
	Graphics gfx;
private:
};