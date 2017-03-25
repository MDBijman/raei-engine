#pragma once
#include "Modules/ECS/System.h"
#include "Game/Components/Position2D.h"
#include "Game/Components/Velocity2D.h"
#include <vector>

namespace Systems
{
	class Movement2D : public MySystem
	{
	public:
		void update(MyECSManager& ecs, double dt) override
		{
			using namespace Components;
			auto entities = ecs.filterEntities<ECS::Filter<Position2D, Velocity2D>>();
			for (auto& entity : entities)
			{
				auto& pos = ecs.getComponent<Position2D>(entity);
				auto& vel = ecs.getComponent<Velocity2D>(entity);

				pos.pos += vel.vel;
			}
		}
	};
}
