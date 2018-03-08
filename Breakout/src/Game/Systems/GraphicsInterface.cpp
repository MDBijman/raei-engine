#include "stdafx.h"
#include "Game/Systems/GraphicsInterface.h"
#include "Modules/Graphics/Core/Renderer.h"
#include "Modules/Graphics/Drawables/Text.h"

void Systems::GraphicsInterface::update(ecs_manager& ecs)
{
	glm::mat4 pv;
	{
		auto result = ecs.filterEntities<ecs::filter<Components::Camera2D>>();
		if (result.entities.size() == 0)
			return;

		auto& camera = ecs.getComponent<Components::Camera2D>(*result.entities.begin());
		pv = camera.camera.getMatrices().view_projection;
	}

	auto frame = graphics->getFrame();

	{ // We need to put this in its own scope so the component locks are deallocated properly
		auto sprites = ecs.filterEntities<ecs::filter<
			components::drawable<shaders::sprite_shader>,
			Components::Position2D,
			Components::Scale2D
			>>();
		for (auto e : sprites.entities)
		{
			auto& sprite = ecs.getComponent<components::drawable<shaders::sprite_shader>>(e);
			auto& pos = ecs.getComponent<Components::Position2D>(e);
			auto& scale2d = ecs.getComponent<Components::Scale2D>(e);

			// Uniform camera matrix
			auto scale = glm::scale(glm::mat4(), glm::vec3(scale2d.scale, 1.0f));
			auto translate = glm::translate(glm::mat4(), glm::vec3(pos.pos, 0.0f));
			auto model = translate * scale;
			glm::mat4 mvp = pv * model;
			sprite.shader().uniforms().upload<0>(mvp);

			frame.add_drawable<shaders::sprite_shader>(sprite);
		}
	}

	{
		auto scores = ecs.filterEntities<ecs::filter<
			components::drawable<speck::graphics::text>,
			Components::Position2D,
			Components::Scale2D
			>>();
		for (auto e : scores.entities)
		{
			auto& text = ecs.getComponent<components::drawable<speck::graphics::text>>(e);
			auto& pos = ecs.getComponent<Components::Position2D>(e);
			auto& scale2d = ecs.getComponent<Components::Scale2D>(e);

			// Uniform camera matrix
			auto scale = glm::scale(glm::mat4(), glm::vec3(scale2d.scale, 1.0f));
			auto translate = glm::translate(glm::mat4(), glm::vec3(pos.pos, 0.0f));
			auto model = translate * scale;
			glm::mat4 mvp = pv * model;
			text.shader().uniforms().upload<0>(mvp);

			frame.add_drawable(text);
		}
	}

	graphics->submitFrame(&frame);
}