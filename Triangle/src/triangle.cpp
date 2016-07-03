#include <chrono>
#include <windows.h>
#include <iostream>
#include "Game.h"

HWND window;
Game* t;

/*
* Creates and initializes a Win32 Context.
*/
void createWindowsContext(HINSTANCE hInstance, WNDPROC WndProc, std::string name, int width, int height)
{
	WNDCLASSEX wndClass;
	wndClass.cbSize        = sizeof(WNDCLASSEX);
	wndClass.style         = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc   = WndProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = hInstance;
	wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName  = NULL;
	wndClass.lpszClassName = name.c_str();
	wndClass.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		std::cout << "Could not register window class!\n";
		fflush(stdout);
		exit(1);
	}

	int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DWORD dwExStyle;
	DWORD dwStyle;
	RECT windowRect;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle   = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	windowRect.left   = (long)screenWidth / 2 - width / 2;
	windowRect.right  = (long)width;
	windowRect.top    = (long)screenHeight / 2 - height / 2;
	windowRect.bottom = (long)height;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	window = CreateWindowEx(0,
		name.c_str(), name.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		windowRect.left, windowRect.top, windowRect.right, windowRect.bottom,
		NULL, NULL, hInstance, NULL);

	if (!window)
	{
		printf("Could not create window!\n");
		fflush(stdout);
		exit(1);
	}

	ShowWindow(window, SW_SHOW);
	SetForegroundWindow(window);
	SetFocus(window);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		exit(0);
		break;
	case WM_KEYDOWN:
		std::cout << wParam << std::endl;

		// Delete button
		if (wParam == VK_ESCAPE)
			exit(0);

		// Move forward or backward
		if (wParam == 'W') 
			t->camera.forward(300.0f, t->dt);
		else if (wParam == 'S')
			t->camera.backward(300.0f, t->dt);

		// Move left or right
		if (wParam == 'A') 
			t->camera.left(300.0f, t->dt);
		else if (wParam == 'D')
			t->camera.right(300.0f, t->dt);
		break;
	case WM_MOUSEMOVE:
		POINT cursor;
		GetCursorPos(&cursor);
		ScreenToClient(hWnd, &cursor);

		POINT middle;
		middle.x = 640;
		middle.y = 360;
		
		POINT delta;
		delta.x = cursor.x - middle.x;
		delta.y = cursor.y - middle.y;

		t->camera.rotate(delta.x * t->rotationSpeed * 20.0f, delta.y * t->rotationSpeed * 20.0f, t->dt);

		ClientToScreen(hWnd, &middle);
		SetCursorPos(middle.x, middle.y);
		break;
	}

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	createWindowsContext(hInstance, WndProc, "triangle", 1280, 720);

	AllocConsole();
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD cCharsWritten;
	freopen("CONOUT$", "w", stdout);

	t = new Game(hInstance, window);
	t->begin();

	MSG msg;
	while (TRUE)
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		t->draw();

		auto endTime = std::chrono::high_resolution_clock::now();
		auto difference = std::chrono::duration<double, std::milli>(endTime - startTime).count();
		t->dt = difference / 1000;
	}

	t->end();
	return 0;
}
