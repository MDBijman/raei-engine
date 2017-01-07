#pragma once
#include "Modules/ECS/Component.h"
#include <glm/vec2.hpp>

namespace Components
{
	class Scale2D : public Component
	{
	public:
		Scale2D() = default;
		Scale2D(glm::vec2 a) : scale(a) {}
		glm::vec2 scale;
	};
}
