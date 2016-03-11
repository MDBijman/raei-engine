#include "VulkanTriangle.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		exit(0);
		break;
	}

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	std::unique_ptr<VkTriangle> application = std::make_unique<VkTriangle>(hInstance, WndProc, "triangle", 1280, 720);

	application->loop();

	return 0;
}
