#pragma once
#include <unordered_map>

namespace Input
{
	namespace Keyboard
	{
		enum KeyStatus
		{
			DOWN, UP, UNKNOWN
		};

		void setKeyDown(int keycode);
		void setKeyUp(int keycode);

		KeyStatus getKeyStatus(int keycode);

		static std::unordered_map<int, KeyStatus> keys;
	}

	namespace Mouse
	{
		void setX(float val);
		void setY(float val);
		float getDX();
		float getDY();

		static float x = 0.0f, y = 0.0f, dx = 0.0f, dy = 0.0f;
	}
}
