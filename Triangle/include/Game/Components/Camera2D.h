#pragma once
#include "Modules/ECS/Component.h"
#include "Modules/Graphics/Graphics.h"

namespace Components
{
	class Camera2D : public Component
	{
	public:
		Camera2D(Graphics::Camera c) : camera(c) {}

		Graphics::Camera camera;
	};
}