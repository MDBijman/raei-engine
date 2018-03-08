#pragma once
#include "Game/World.h"

namespace graphics
{
	class Camera;
	class Renderer;
}

namespace game
{
	world load_menu(graphics::Camera& graphics_camera, graphics::Renderer& graphics);
}