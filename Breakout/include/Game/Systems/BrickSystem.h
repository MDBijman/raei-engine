#pragma once
#include "Game/EventConfig.h"
#include "Game/Events/BrickEvents.h"
#include "Game/ECSConfig.h"

namespace systems
{
	class brick_system : public MySystem
	{
		events::subscriber<events::collision>& subscriber;

	public:
		brick_system(events::subscriber<events::collision>& sub) : subscriber(sub) {}

		void update(ecs_manager& ecs) override
		{
			auto&[lock, bricks] = ecs.filterEntities<ecs::filter<components::brick>>();

			while (subscriber.size() > 0)
			{
				auto hit = subscriber.pop();
				if (bricks.find(hit->b) != bricks.end())
					ecs.removeEntity(hit->b, ecs::option::defered);
				else if (bricks.find(hit->a) != bricks.end())
					ecs.removeEntity(hit->a, ecs::option::defered);
			}
		}
	};
}