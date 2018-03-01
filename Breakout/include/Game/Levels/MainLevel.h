#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"
#include "Game/Systems/Systems.h"
#include "Game/Events/BrickEvents.h"

#include "Modules/Graphics/Core/Camera.h"
#include "Modules/TemplateUtils/OrderedTuple.h"

namespace game
{
	namespace detail
	{
		std::vector<graphics::data::OrderedTuple<
			graphics::data::Vec2<0, 0>,
			graphics::data::Vec2<1, sizeof(float) * 2>>
			> create_block()
		{
			return {
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
			};
		}
	}

	void load(std::shared_ptr<ecs_manager> ecs, std::shared_ptr<event_manager>& events,
		graphics::Camera& graphics_camera, graphics::Renderer& graphics)
	{
		auto cameraEntity = ecs->createEntity();
		auto& pos = ecs->addComponent(cameraEntity, Components::Position3D(0.0, 0.0, 1.0));
		ecs->addComponent(cameraEntity, Components::Orientation3D());
		auto& camera = ecs->addComponent(cameraEntity, Components::Camera2D(graphics_camera));
		camera.camera.moveTo(pos.pos, glm::vec3());

		auto sprite = ecs->createEntity();
		{
			ecs->addComponent(sprite, Components::Position2D(0.0, 0.8));
			ecs->addComponent(sprite, Components::Velocity2D(0.0, 0.0));
			ecs->addComponent(sprite, Components::Scale2D(.2, 0.05));
			ecs->addComponent(sprite, Components::Input(1.0f));
			ecs->addComponent(sprite, components::collider());
			ecs->addComponent(sprite, components::paddle());

			auto attributes = Components::sprite_attributes(detail::create_block());
			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });
			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& drawable = ecs->addComponent(sprite, components::drawable<sprite_shader>(
				graphics.resources.create_drawable(
					sprite_shader(std::move(attributes), std::move(indices), std::move(uniform)),
					graphics.drawPass, graphics.frameBuffers
				)));
		}

		auto ball = ecs->createEntity();
		{
			ecs->addComponent(ball, Components::Position2D(0.0, 0.7));
			ecs->addComponent(ball, Components::Velocity2D(0.0, 1.6));
			ecs->addComponent(ball, Components::Scale2D(.03, .03));
			ecs->addComponent(ball, components::collider());
			ecs->addComponent(ball, components::ball());

			auto attributes = Components::sprite_attributes(detail::create_block());
			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });
			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& drawable = ecs->addComponent(ball, components::drawable<sprite_shader>(
				graphics.resources.create_drawable(
					sprite_shader(std::move(attributes), std::move(indices), std::move(uniform)),
					graphics.drawPass, graphics.frameBuffers
				)));
		}

		// Bricks
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				auto block = ecs->createEntity();

				float width = 0.135f;
				float gap = 0.05f;

				ecs->addComponent(block, Components::Position2D(
					-0.9 + (width + gap) * i + 0.5f * width,
					-0.85 + (double)j / 8.0
				));
				ecs->addComponent(block, Components::Scale2D(width, 1.0f / 15.0f));
				ecs->addComponent(block, components::collider());
				ecs->addComponent(block, components::brick());

				auto attributes = Components::sprite_attributes(detail::create_block());
				auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });
				auto uniform = Components::sprite_uniforms(*graphics.context, {
					graphics.resources.create_buffer<glm::mat4>(
						camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
					graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
						vk::ShaderStageFlagBits::eFragment)
					});

				auto& drawable = ecs->addComponent(block, components::drawable<sprite_shader>(
					graphics.resources.create_drawable(
						sprite_shader(std::move(attributes), std::move(indices), std::move(uniform)),
						graphics.drawPass, graphics.frameBuffers
					)));
			}
		}

		// Top Wall
		{
			auto top_wall = ecs->createEntity();

			ecs->addComponent(top_wall, Components::Position2D(0.0, -1.0));
			ecs->addComponent(top_wall, Components::Scale2D(2.0, 1.0 / 15.0));
			ecs->addComponent(top_wall, components::collider());
			ecs->addComponent(top_wall, components::wall());

			auto attributes = Components::sprite_attributes(detail::create_block());
			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });
			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});


			auto& drawable = ecs->addComponent(top_wall, components::drawable<sprite_shader>(
				graphics.resources.create_drawable(
					sprite_shader(std::move(attributes), std::move(indices), std::move(uniform)),
					graphics.drawPass, graphics.frameBuffers
				)));
		}

		// Left Wall
		{
			auto left_wall = ecs->createEntity();

			ecs->addComponent(left_wall, Components::Position2D(-1.0, 0.0));
			ecs->addComponent(left_wall, Components::Scale2D(1.0 / 15.0, 2.0));
			ecs->addComponent(left_wall, components::collider());
			ecs->addComponent(left_wall, components::wall());

			auto attributes = Components::sprite_attributes(detail::create_block());
			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });
			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& drawable = ecs->addComponent(left_wall, components::drawable<sprite_shader>(
				graphics.resources.create_drawable(
					sprite_shader(std::move(attributes), std::move(indices), std::move(uniform)),
					graphics.drawPass, graphics.frameBuffers
				)));
		}

		// Right Wall
		{
			auto left_wall = ecs->createEntity();

			ecs->addComponent(left_wall, Components::Position2D(1.0, 0.0));
			ecs->addComponent(left_wall, Components::Scale2D(1.0 / 15.0, 2.0));
			ecs->addComponent(left_wall, components::collider());
			ecs->addComponent(left_wall, components::wall());

			auto attributes = Components::sprite_attributes(detail::create_block());
			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });
			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& drawable = ecs->addComponent(left_wall, components::drawable<sprite_shader>(
				graphics.resources.create_drawable(
					sprite_shader(std::move(attributes), std::move(indices), std::move(uniform)),
					graphics.drawPass, graphics.frameBuffers
				)));
		}

		// Score
		{
			auto score = ecs->createEntity();

			ecs->addComponent(score, components::score());
			ecs->addComponent(score, Components::Position2D(-1.8361, -.99));
			ecs->addComponent(score, Components::Scale2D(.3, .1));

			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/fonts/vcr_osd_mono.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto attributes = Components::sprite_attributes({
				// Quad 1
				{
					{ 0.0f, 0.0f },
					{ 0.0f, 0.0f }
				},
				{
					{ 0.0f, 1.0f },
					{ 0.0f, 1.0f }
				},
				{
					{ 1.0f, 0.0f },
					{ 1.0f, 0.0f }
				},
				{
					{ 1.0f, 1.0f },
					{ 1.0f, 1.0f }
				},
				// Quad 2
				{
					{ 1.0f, 0.0f },
					{ 0.0f, 0.0f }
				},
				{
					{ 1.0f, 1.0f },
					{ 0.0f, 1.0f }
				},
				{
					{ 2.0f, 0.0f },
					{ 1.0f, 0.0f }
				},
				{
					{ 2.0f, 1.0f },
					{ 1.0f, 1.0f }
				},
				// Quad 4 
				{
					{ 2.0f, 0.0f },
					{ 0.0f, 0.0f }
				},
				{
					{ 2.0f, 1.0f },
					{ 0.0f, 1.0f }
				},
				{
					{ 3.0f, 0.0f },
					{ 1.0f, 0.0f }
				},
				{
					{ 3.0f, 1.0f },
					{ 1.0f, 1.0f }
				}
				});

			auto indices = Components::sprite_indices({
				// Quad 1
				0, 1, 2, 2, 1, 3,
				// Quad 2 
				4, 5, 6, 6, 5, 7,
				// Quad 3
				8, 9, 10, 10, 9, 11,
				});

			auto& drawable = ecs->addComponent(score, components::drawable<sprite_shader>(
				graphics.resources.create_drawable(
					sprite_shader(std::move(attributes), std::move(indices), std::move(uniform)),
					graphics.drawPass, graphics.frameBuffers
				)));
		}

		{
			auto sg = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(sg, std::make_unique<Systems::Exit>());
			ecs->get_system_manager().add_to_group(sg, std::make_unique<Systems::Input>());
			ecs->get_system_manager().add_to_group(sg, std::make_unique<Systems::CameraSystem>());
			ecs->get_system_manager().add_to_group(sg, std::make_unique<systems::game_rule_system>());
			ecs->get_system_manager().add_to_group(sg, std::make_unique<systems::fps_system>());

			auto render_thread = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(render_thread, 
				std::make_unique<Systems::GraphicsInterface>(&graphics));
			ecs->get_system_manager().add_to_group(render_thread, 
				std::make_unique<Systems::SpriteShaderSystem>(*graphics.context));

			auto pt = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(pt, std::make_unique<Systems::Movement2D>());
			ecs->get_system_manager().add_to_group(pt, std::make_unique<systems::collision_system>(
				events->new_publisher<events::collision>()));

			auto post_collisions = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(post_collisions, std::make_unique<systems::brick_system>(
				events->new_subscriber<events::collision>()));
			ecs->get_system_manager().add_to_group(post_collisions, std::make_unique<systems::score_system>(
				events->new_subscriber<events::collision>(), *graphics.context));
			ecs->get_system_manager().add_to_group(post_collisions, std::make_unique<systems::ball_system>(
				events->new_subscriber<events::collision>()));
			ecs->get_system_manager().add_to_group(post_collisions, std::make_unique<systems::powerup_system>(
				events->new_subscriber<events::collision>(), graphics, camera.camera));
		}
	}
}