#include <Windows.h>
#include <iostream>
#include "Game/Game.h"
#include "Modules/IO/Input.h"

HWND window;

/*
* Creates and initializes a Win32 Context.
*/
void createWindowsContext(HINSTANCE hInstance, WNDPROC WndProc, std::string name, int width, int height)
{
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = name.c_str();
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		std::cout << "Could not register window class!\n";
		fflush(stdout);
		exit(1);
	}

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DWORD dwExStyle;
	DWORD dwStyle;
	RECT windowRect;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	windowRect.left = (long)screenWidth / 2 - width / 2;
	windowRect.right = (long)width;
	windowRect.top = (long)screenHeight / 2 - height / 2;
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
	ShowCursor(FALSE);
	SetForegroundWindow(window);
	SetFocus(window);
}

// Windows callback
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		exit(0);
	case WM_KEYUP:
		IO::Keyboard::setKeyUp(wParam);
		break;
	case WM_KEYDOWN:
		IO::Keyboard::setKeyDown(wParam);
		break;
	case WM_MOUSEMOVE:
		POINT cursor;
		GetCursorPos(&cursor);
		ScreenToClient(hWnd, &cursor);

		POINT middle;
		middle.x = 640;
		middle.y = 360;

		IO::Mouse::setX(cursor.x);
		IO::Mouse::setY(cursor.y);

		ClientToScreen(hWnd, &middle);
		SetCursorPos(middle.x, middle.y);
		break;
	default: break;
	}

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	createWindowsContext(hInstance, WndProc, "triangle", 1280, 720);

	AllocConsole();
	//HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	freopen("CONOUT$", "w", stdout);

	Game game(hInstance, window);
	game.run();

	return 0;
}
