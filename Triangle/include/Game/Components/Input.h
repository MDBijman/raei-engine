#pragma once
#include "Modules\ECS\ECS.h"

namespace Components
{
	class Input : public Component
	{
	public:
		enum KeyboardMode
		{
			WASD
		};

		enum MouseMode
		{
			FIRST_PERSON
		};

		Input(KeyboardMode km, MouseMode mm) : keyboardMode(km), mouseMode(mm) {}

		Input()
		{
			keyboardMode = KeyboardMode::WASD;
			mouseMode = MouseMode::FIRST_PERSON;
		}

		KeyboardMode keyboardMode;
		MouseMode mouseMode;
	};
}
