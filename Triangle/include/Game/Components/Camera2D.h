#pragma once
#include "Modules/ECS/Component.h"
namespace Graphics { class Camera; }

namespace Components
{
	class Camera2D : public ECS::Component
	{
	public:
		Camera2D(Graphics::Camera& c) : camera(c) {}

		Graphics::Camera& camera;
	};
}