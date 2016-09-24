#pragma once
#include "Modules/ECS/Component.h"
#include "Modules/Graphics/Graphics.h"

namespace Components
{
	class Camera2D : public Component, public Camera
	{
		using Camera::Camera;
	};
}