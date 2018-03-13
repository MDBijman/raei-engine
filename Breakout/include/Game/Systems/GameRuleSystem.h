#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"

namespace systems
{
	class game_rule_system : public MySystem
	{
		const events::publisher<events::switch_world>& publisher;

	public:
		game_rule_system(const events::publisher<events::switch_world>& pub) : publisher(pub) {}

		void update(ecs_manager& ecs) override;
	};
}