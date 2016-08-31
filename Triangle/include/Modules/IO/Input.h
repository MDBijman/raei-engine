#pragma once
#include <unordered_map>

namespace IO
{
	namespace Keyboard
	{
		enum KeyStatus
		{
			DOWN, UP, UNKNOWN
		};

		void setKeyDown(uint64_t keycode);
		void setKeyUp(uint64_t keycode);

		KeyStatus getKeyStatus(uint64_t keycode);

		static std::unordered_map<uint64_t, KeyStatus> keys;
	}

	namespace Mouse
	{
		void setX(uint32_t val);
		void setY(uint32_t val);
		uint32_t getDX();
		uint32_t getDY();

		static uint32_t x = 0, y = 0, dx = 0, dy = 0;
	}

	namespace Polling
	{
		void update();
	}
}
