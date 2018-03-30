#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"

namespace systems
{
	class game_rule_system : public MySystem
	{
		const events::publisher<events::switch_world>& publisher;
		const events::publisher<events::new_score>& score_publisher;

	public:
		game_rule_system(const events::publisher<events::switch_world>& pub, 
			const events::publisher<events::new_score>& pub2) : publisher(pub), score_publisher(pub2) {}

		void update(ecs_manager& ecs) override;
	};
}