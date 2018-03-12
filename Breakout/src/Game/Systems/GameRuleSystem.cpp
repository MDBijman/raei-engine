#include "stdafx.h"
#include "Game/Systems/GameRuleSystem.h"

void systems::game_rule_system::update(ecs_manager& ecs)
{
	auto balls = ecs.filterEntities<ecs::filter<components::ball>>();
	bool alive = false;
	for (auto ball : balls.entities)
	{
		if (ecs.getComponent<Components::Position2D>(ball).pos.y < 1.1)
		{
			alive = true;
		}
		else
		{
			ecs.removeEntity(ball, ecs::option::defered);
		}
	}

	if (!alive) this->publisher.broadcast(events::switch_world(game::worlds::MAIN_MENU));
}