#pragma once
#include "Modules\ECS\ECS.h"
#include "Game\GameConfig.h"

namespace Systems
{
	class GraphicsInterface : public MySystem
	{
	public:
		virtual void update(MyECSManager& ecs, double dt)
		{
			using namespace Components;
			auto entities = ecs.filterEntities<Filter<Components::Texture, Components::Mesh>>();
			for (auto entity : entities)
			{
				auto& texture = ecs.getComponent<Components::Texture>(entity);
				auto& mesh = ecs.getComponent<Components::Mesh>(entity);
				
			}
		}
	};
}
