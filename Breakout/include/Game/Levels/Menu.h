#pragma once
#include <memory>
#include "Modules/Graphics/Graphics.h"
#include "../ECSConfig.h"
#include "../EventConfig.h"

namespace game
{
	void load(std::shared_ptr<ecs_manager> ecs, std::shared_ptr<event_manager>& events,
		graphics::Camera& graphics_camera, graphics::Renderer& graphics)
	{
		auto title = ecs->createEntity();
		{
			ecs->addComponent(title, Components::Position2D(0.0, 0.0));
			ecs->addComponent(title, Components::Scale2D(.3, .3));
			auto attributes = graphics.resources.create_attributes<Components::sprite_attributes>({
				{
					{ -.5f, -.5f },
					{ 0.0f, 0.0f }
				},
				{
					{ -.5f, .5f },
					{ 0.0f, 1.0f }
				},
				{
					{ .5f, -.5f },
					{ 1.0f, 0.0f }
				},
				{
					{ .5f, .5f },
					{ 1.0f, 1.0f }
				}
				});
			auto indices = graphics.resources.create_indices({ 0, 1, 2, 2, 1, 3 });
			auto uniform = graphics.resources.create_uniform<Components::sprite_uniforms>({
				graphics.resources.create_buffer<glm::mat4>(
					graphics_camera.getMatrices().projection * graphics_camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& drawable = ecs->addComponent(title, components::drawable<sprite_shader>(
				graphics.resources.create_drawable(
					sprite_shader(std::move(attributes), std::move(indices), std::move(uniform))
				)));
		}
	}
}