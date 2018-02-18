#pragma once
#include "../ECSConfig.h"

namespace systems
{
	class game_rule_system : public MySystem
	{
	public:
		game_rule_system(uint32_t ball)
			: ball(ball) {}

		void update(ecs_manager& ecs) override
		{
			auto& ball_pos = ecs.getComponent<Components::Position2D>(ball);

			if (ball_pos.pos.y > 1.1)
			{
				exit(0);
			}
		}

	private:
		uint32_t ball;
	};
}