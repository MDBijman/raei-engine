#pragma once
#include "Modules/ECS/ECS.h"
#include "Game/Components/Position2D.h"
#include "Game/GameConfig.h"

namespace Systems
{
	class Movement2D : public MySystem
	{
	public:
		void update(MyECSManager& ecs, double dt) override
		{
			using namespace Components;
			auto entities = ecs.filterEntities<Filter<Position2D>>();
			for (auto& entity : entities)
			{
				auto& pos = ecs.getComponent<Position2D>(entity);
			}
		}
	};
}
