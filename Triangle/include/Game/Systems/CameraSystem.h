#pragma once
#include "Modules/ECS/ECS.h"
#include "Game/GameConfig.h"


namespace Systems
{
	class CameraSystem : public MySystem
	{
	public:
		void update(MyECSManager& ecs, float dt)
		{
			auto entities = ecs.filterEntities<Filter<Components::Camera2D, Components::Position2D>>();
		
			for(auto entity : entities)
			{
				auto& camera = ecs.getComponent<Components::Camera2D>(entity);
				auto& position = ecs.getComponent<Components::Position2D>(entity);

				camera.camera.moveTo(glm::vec3(position.pos.x, position.pos.y, 0.0f), glm::vec3());
			}
		}
	};
}