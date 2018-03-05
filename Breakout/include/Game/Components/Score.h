#pragma once
#include "Game/ECSConfig.h"
#include "Modules/Graphics/Drawables/Text.h"

namespace components
{
	struct score : public ecs::Component, public speck::graphics::drawable<speck::graphics::text>
	{
		score(int count, speck::graphics::drawable<speck::graphics::text> drawable) :
			count(count), drawable(std::move(drawable)) {}

		int count;
	};
}