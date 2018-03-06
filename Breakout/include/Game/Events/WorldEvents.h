#pragma once
#include "Game/WorldConfig.h"

namespace events
{
	struct switch_world
	{
		switch_world(game::worlds w) : to(w) {}
		game::worlds to;
	};
}