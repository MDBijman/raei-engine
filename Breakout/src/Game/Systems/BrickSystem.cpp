#include "stdafx.h"
#include "Game/Systems/BrickSystem.h"

void systems::brick_system::update(ecs_manager& ecs)
{
	auto bricks = ecs.filterEntities<ecs::filter<components::brick>>();

	while (subscriber.size() > 0)
	{
		auto hit = subscriber.pop();
		if (bricks.entities.find(hit->b) != bricks.entities.end())
			ecs.removeEntity(hit->b, ecs::option::defered);
		else if (bricks.entities.find(hit->a) != bricks.entities.end())
			ecs.removeEntity(hit->a, ecs::option::defered);
	}
}