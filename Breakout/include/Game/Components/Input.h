#pragma once
#include "Modules/ECS/Component.h"

namespace Components
{
	class Input : public ecs::Component
	{
	public:
		float speed;
		Input() = default;
		Input(float speed) : speed(speed) {}
	};
}
