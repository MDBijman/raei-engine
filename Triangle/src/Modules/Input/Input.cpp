#include "Modules\Input\Input.h"

namespace Input
{
	namespace Keyboard
	{
		void setKeyDown(int keycode)
		{
			if (keys.find(keycode) == keys.end())
				keys.insert(std::make_pair(keycode, KeyStatus::DOWN));
			else
				keys.at(keycode) = KeyStatus::DOWN;
		}

		void setKeyUp(int keycode)
		{
			if (keys.find(keycode) == keys.end())
				keys.insert(std::make_pair(keycode, KeyStatus::UP));
			else
				keys.at(keycode) = KeyStatus::UP;
		}

		KeyStatus getKeyStatus(int keycode)
		{
			if (keys.find(keycode) == keys.end())
				return KeyStatus::UNKNOWN;
			return keys.at(keycode);
		}
	}

	namespace Mouse
	{
		void setX(float val)
		{
			x = val;
		}

		void setY(float val)
		{
			y = val;
		}

		float getDX()
		{
			return x - 640;
		}

		float getDY()
		{
			return y - 360;
		}
	}
}