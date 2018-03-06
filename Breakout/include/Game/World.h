#pragma once
#include "ECSConfig.h"
#include "EventConfig.h"

namespace game
{
	struct world
	{
		ecs_manager ecs;
		event_manager events;

		world(ecs_manager&& ecs, event_manager&& events) : 
			ecs(std::move(ecs)), 
			events(std::move(events))
		{}

		world(world&& o) : 
			ecs(std::move(o.ecs)),
			events(std::move(o.events))
		{}

		void update()
		{
			this->ecs.update();
		}
	};
}
