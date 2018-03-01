#pragma once
#include "Modules/ECS/System.h"
#include "Modules/Graphics/Core/VulkanContext.h"
#include "Game/Components/SpriteShader.h"
#include "Game/Components/Position2D.h"
#include "Game/Components/Scale2D.h"

namespace Systems
{
	class SpriteShaderSystem : public MySystem
	{
	public:
		SpriteShaderSystem(graphics::VulkanContext& context) : context(context) {}

		void update(ecs_manager& ecs) override
		{
			glm::mat4 pv;
			{
				auto result = ecs.filterEntities<ecs::filter<Components::Camera2D>>();
				if (result.entities.size() == 0)
					return;

				auto& camera = ecs.getComponent<Components::Camera2D>(*result.entities.begin());
				pv = camera.camera.getMatrices().view_projection;
			}

			auto result = ecs.filterEntities<ecs::filter<components::drawable<sprite_shader>, Components::Position2D,
				Components::Scale2D >>();

			for (auto entity : result.entities)
			{
				auto& drawable = ecs.getComponent<components::drawable<sprite_shader>>(entity);
				auto& pos = ecs.getComponent<Components::Position2D>(entity);
				auto& scale2d = ecs.getComponent<Components::Scale2D>(entity);

				auto scale = glm::scale(glm::mat4(), glm::vec3(scale2d.scale, 1.0f));
				auto translate = glm::translate(glm::mat4(), glm::vec3(pos.pos, 0.0f));
				auto model = translate * scale;

				glm::mat4 mvp = pv * model;

				drawable.shader().uniforms().upload<0>(context, mvp);
			}
		}

		graphics::VulkanContext& context;
	};
}