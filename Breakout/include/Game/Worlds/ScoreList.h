#pragma once
#include "Game/WorldConfig.h"
#include "Game/World.h"
#include "Game/Scores.h"

namespace game
{
	world load_scorelist(graphics::Camera& graphics_camera, graphics::Renderer& graphics, game::scores& scores);
}