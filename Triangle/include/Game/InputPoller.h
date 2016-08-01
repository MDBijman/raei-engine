#pragma once

class InputPoller
{
public:
	void update()
	{
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT)
			exit(0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		Input::Mouse::setX(640);
		Input::Mouse::setY(360);
	}


};
