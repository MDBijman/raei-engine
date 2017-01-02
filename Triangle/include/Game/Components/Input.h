#pragma once
#include "Modules/ECS/ECS.h"

namespace Components
{
	class Input : public Component
	{
	public:
		float speed;
		Input() = default;
		Input(float speed) : speed(speed) {}
	};
}
