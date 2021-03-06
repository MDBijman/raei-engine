#pragma once
#include "Modules/ECS/System.h"

namespace Systems
{
	class CameraSystem : public MySystem
	{
	public:
		void update(ecs_manager& ecs) override
		{
			auto result = ecs.filterEntities<ecs::filter<Components::Camera2D, Components::Position3D,
				Components::Orientation3D>>();
			
			for(auto entity : result.entities)
			{
				auto& camera = ecs.getComponent<Components::Camera2D>(entity);
				auto& position = ecs.getComponent<Components::Position3D>(entity);
				auto& orientation = ecs.getComponent<Components::Orientation3D>(entity);

				camera.camera.moveTo(position.pos, glm::vec3());
			}
		}
	};
}