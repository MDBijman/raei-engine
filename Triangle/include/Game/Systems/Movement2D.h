#pragma once
#include <vector>

#include "Game/Components/Position2D.h"
#include "Game/Components/Velocity2D.h"

#include "Modules/ECS/System.h"
#include "Modules/Time/Timer.h"

namespace Systems
{
	class Movement2D : public MySystem
	{
		Time::Timer2 timer;

	public:
		Movement2D()
		{
			timer.zero();
		}

		void update(ecs_manager& ecs) override
		{
			using namespace Components;
			auto&[lock, entities] = ecs.filterEntities<ecs::filter<Position2D, Velocity2D>>();
			for (auto& entity : entities)
			{
				auto& pos = ecs.getComponent<Position2D>(entity);
				auto& vel = ecs.getComponent<Velocity2D>(entity);

				pos.pos += vel.vel * static_cast<float>(timer.dt());
			}
			timer.zero();
		}
	};
}
