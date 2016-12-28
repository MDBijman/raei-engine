#pragma once
#include "Modules/ECS/ECS.h"
#include "Game/GameConfig.h"

namespace Systems
{
	class GraphicsUpdateSystem : public MySystem
	{
	public:
		void update(MyECSManager& ecs, double dt) override
		{
			auto entities = ecs.filterEntities<Filter<Components::SpriteShader, Components::Position2D>>();

			for(auto entity : entities)
			{
				auto& shader = ecs.getComponent<Components::SpriteShader>(entity);
				auto& pos = ecs.getComponent<Components::Position2D>(entity);
			}
		}
	};
}