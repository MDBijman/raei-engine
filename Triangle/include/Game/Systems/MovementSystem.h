#pragma once
#include "Modules\ECS\ECS.h"
#include "Game\Components\Position3D.h"
#include "Game\Components\Velocity3D.h"
#include "Game\GameConfig.h"

namespace Systems
{
	class MovementSystem : public MySystem
	{
	public:
		virtual void update(MyECSManager& ecs, double dt)
		{
			using namespace Components;
			auto entities = ecs.filterEntities<Filter<Position3D, Velocity3D>>();
			for (auto& entity : entities)
			{
				auto& pos = ecs.getComponent<Position3D>(entity);
				auto& vel = ecs.getComponent<Velocity3D>(entity);
				pos.pos += vel.vel;
			}
		}
	};
}
