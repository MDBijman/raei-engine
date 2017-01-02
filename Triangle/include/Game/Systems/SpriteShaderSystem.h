#pragma once
#include "Modules/ECS/ECS.h"
#include "Game/GameConfig.h"

namespace Systems
{
	class SpriteShaderSystem : public MySystem
	{
	public:
		SpriteShaderSystem(Graphics::VulkanContext& context) : context(context) {}

		void update(MyECSManager& ecs, double dt) override
		{
			auto cameras = ecs.filterEntities<Filter<Components::Camera2D>>();
			if(cameras.size() == 0)
				return;

			auto& camera = ecs.getComponent<Components::Camera2D>(cameras.at(0));

			auto entities = ecs.filterEntities<Filter<Components::SpriteShader, Components::Position2D>>();

			for(auto entity : entities)
			{
				auto& shader = ecs.getComponent<Components::SpriteShader>(entity);
				auto& pos = ecs.getComponent<Components::Position2D>(entity);

				glm::mat4 newMatrix = camera.camera.getMatrices().projection * camera.camera.getMatrices().view;
				camera.camera.move(glm::vec3(0.0f, -10000.0f, 0.0f), 1.0f);
				shader.getUniforms().upload<0>(context, newMatrix);
			}
		}

		Graphics::VulkanContext& context;
	};
}