#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"
#include "Game/WorldConfig.h"

namespace systems
{
	class score_list_system : public MySystem
	{
		events::publisher<events::switch_world>& publisher;
	public:
		score_list_system(events::publisher<events::switch_world>& pub);
		void update(ecs_manager& ecs) override;
	};
}
