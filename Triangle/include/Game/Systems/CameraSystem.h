#pragma once
#include "Modules/ECS/System.h"

namespace Systems
{
	class CameraSystem : public MySystem
	{
	public:
		void update(MyECSManager& ecs, double dt) override
		{
			auto entities = ecs.filterEntities<ECS::Filter<Components::Camera2D, Components::Position3D, Components::Orientation3D>>();
		
			for(auto entity : entities)
			{
				auto& camera = ecs.getComponent<Components::Camera2D>(entity);
				auto& position = ecs.getComponent<Components::Position3D>(entity);
				auto& orientation = ecs.getComponent<Components::Orientation3D>(entity);

				camera.camera.moveTo(position.pos, glm::vec3());
			}
		}
	};
}