#include "Modules/IO/Input.h"
#include <Windows.h>

namespace IO
{
	namespace Keyboard
	{
		void setKeyDown(uint64_t keycode)
		{
			if (keys.find(keycode) == keys.end())
				keys.insert(std::make_pair(keycode, KeyStatus::DOWN));
			else
				keys.at(keycode) = KeyStatus::DOWN;
		}

		void setKeyUp(uint64_t keycode)
		{
			if (keys.find(keycode) == keys.end())
				keys.insert(std::make_pair(keycode, KeyStatus::UP));
			else
				keys.at(keycode) = KeyStatus::UP;
		}

		KeyStatus getKeyStatus(uint64_t keycode)
		{
			if (keys.find(keycode) == keys.end())
				return KeyStatus::UNKNOWN;
			return keys.at(keycode);
		}
	}

	namespace Mouse
	{
		void setX(uint32_t val)
		{
			x = val;
			dx = x - 640;
		}

		void setY(uint32_t val)
		{
			y = val;
			dy = y - 360;
		}

		uint32_t getDX()
		{
			return dx;
		}

		uint32_t getDY()
		{
			return dy;
		}
	}

	namespace Polling
	{
		void update()
		{
			MSG msg;
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			if (msg.message == WM_QUIT)
				exit(0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}