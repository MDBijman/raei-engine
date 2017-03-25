#pragma once
#include "Modules/ECS/System.h"
#include "Modules/Graphics/VulkanWrappers/VulkanContext.h"
#include "Game/Components/SpriteShader.h"
#include "Game/Components/Position2D.h"
#include "Game/Components/Scale2D.h"

namespace Systems
{
	class SpriteShaderSystem : public MySystem
	{
	public:
		SpriteShaderSystem(Graphics::VulkanContext& context) : context(context) {}

		void update(MyECSManager& ecs, double dt) override
		{
			auto cameras = ecs.filterEntities<ECS::Filter<Components::Camera2D>>();
			if(cameras.size() == 0)
				return;
			
			auto& camera = ecs.getComponent<Components::Camera2D>(cameras.at(0));

			auto entities = ecs.filterEntities<ECS::Filter<Components::SpriteShader, Components::Position2D, Components::Scale2D>>();

			for(auto entity : entities)
			{
				auto& shader = ecs.getComponent<Components::SpriteShader>(entity);
				auto& pos = ecs.getComponent<Components::Position2D>(entity);
				auto& scale2d = ecs.getComponent<Components::Scale2D>(entity);

				auto scale = glm::scale(glm::mat4(), glm::vec3(scale2d.scale, 1.0f));
				auto translate = glm::translate(glm::mat4(), glm::vec3(pos.pos, 0.0f));
				auto model = translate * scale;
				
				
				glm::mat4 mvp = camera.camera.getMatrices().projection * camera.camera.getMatrices().view * model;
				shader.getUniforms().upload<0>(context, mvp);
			}
		}

		Graphics::VulkanContext& context;
	};
}