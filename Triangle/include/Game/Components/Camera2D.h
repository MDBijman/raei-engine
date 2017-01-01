#pragma once
#include "Modules/ECS/Component.h"
#include "Modules/Graphics/Graphics.h"

namespace Components
{
	class Camera2D : public Component, public Graphics::PerspectiveCamera
	{
	public:
		Camera2D(float fov, float aspect, float n, float f) : Graphics::PerspectiveCamera(fov, aspect, n, f)
		{}
	};
}