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
}
