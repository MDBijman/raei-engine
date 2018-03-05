#pragma once
#include "Game/ECSConfig.h"
#include "Modules/Graphics/Drawables/Text.h"

namespace components
{
	struct score : public ecs::Component
	{
		score(int count) : count(count) {}

		int count;
	};
}