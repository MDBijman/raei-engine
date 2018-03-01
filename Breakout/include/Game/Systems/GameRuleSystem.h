#pragma once
#include "../ECSConfig.h"


namespace systems
{
	class game_rule_system : public MySystem
	{
	public:
		void update(ecs_manager& ecs) override
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

			if (!alive) exit(0);
		}
	};
}