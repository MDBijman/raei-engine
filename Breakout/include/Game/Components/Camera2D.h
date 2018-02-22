#pragma once
#include "Modules/ECS/Component.h"
namespace graphics { class Camera; }

namespace Components
{
	class Camera2D : public ecs::Component
	{
	public:
		Camera2D(graphics::Camera& c) : camera(c) {}

		graphics::Camera& camera;
	};
}