#pragma once
#include <memory>
#include "Modules/Graphics/Graphics.h"
#include "Modules/Graphics/Drawables/Text.h"
#include "../Systems/Systems.h"
#include "../ECSConfig.h"
#include "../EventConfig.h"

namespace game
{
	std::vector<graphics::data::OrderedTuple<
		graphics::data::Vec2<0, 0>,
		graphics::data::Vec2<1, sizeof(float) * 2>>
		> create_block()
	{
		return {
			{
				{ 0.f, 0.0f },
				{ 0.0f, 0.0f }
			},
			{
				{ 0.f, 1.0f },
				{ 0.0f, 1.0f }
			},
			{
				{ 1.0f, 0.0f },
				{ 1.0f, 0.0f }
			},
			{
				{ 1.0f, 1.0f },
				{ 1.0f, 1.0f }
			}
		};
	}

	void load_menu(std::shared_ptr<ecs_manager> ecs, std::shared_ptr<event_manager> events,
		graphics::Camera& graphics_camera, graphics::Renderer& graphics)
	{
		{
			auto camera_entity = ecs->createEntity();
			auto& pos = ecs->addComponent(camera_entity, Components::Position3D(0.0, 0.0, 1.0));
			ecs->addComponent(camera_entity, Components::Orientation3D());
			auto& camera = ecs->addComponent(camera_entity, Components::Camera2D(graphics_camera));
			camera.camera.moveTo(pos.pos, glm::vec3());
		}

		{
			auto title = ecs->createEntity();
			ecs->addComponent(title, Components::Position2D(-1.2, -0.7));
			ecs->addComponent(title, Components::Scale2D(.3, .3));
			ecs->addComponent(title, components::drawable<speck::graphics::text>(
				graphics.factory.create_text("BREAKOUT", graphics_camera)));
		}

		{
			auto start = ecs->createEntity();
			ecs->addComponent(start, Components::Position2D(-0.3, -0.2));
			ecs->addComponent(start, Components::Scale2D(.1, .1));
			ecs->addComponent(start, components::drawable<speck::graphics::text>(
				graphics.factory.create_text("START", graphics_camera)));
		}

		{
			auto scores = ecs->createEntity();
			ecs->addComponent(scores, Components::Position2D(-0.3, 0.0));
			ecs->addComponent(scores, Components::Scale2D(.1, .1));
			ecs->addComponent(scores, components::drawable<speck::graphics::text>(
				graphics.factory.create_text("SCORES", graphics_camera)));
		}

		{
			auto exit = ecs->createEntity();
			ecs->addComponent(exit, Components::Position2D(-0.3, 0.2));
			ecs->addComponent(exit, Components::Scale2D(.1, .1));
			ecs->addComponent(exit, components::drawable<speck::graphics::text>(
				graphics.factory.create_text("QUIT", graphics_camera)));
		}

		{
			auto pointer = ecs->createEntity();
			ecs->addComponent(pointer, Components::Position2D(-.5f, -0.2f));
			ecs->addComponent(pointer, Components::Scale2D(.1f, .1f));
			ecs->addComponent(pointer, components::drawable<sprite_shader>(
				graphics.resources.create_drawable(sprite_shader(
					graphics.resources.create_attributes<sprite_attributes>(create_block()),
					graphics.resources.create_indices({ 0, 1, 2, 2, 1, 3 }),
					graphics.resources.create_uniform<sprite_uniforms>({
						graphics.resources.create_buffer(graphics_camera.getMatrices().projection *
							graphics_camera.getMatrices().view * glm::mat4(), 0),
						graphics.resources.create_texture("./res/textures/pointer.dds", vk::Format::eBc3UnormBlock, 1,
							vk::ShaderStageFlagBits::eFragment)
					}))
				)
			));
		}

		{
			auto render_group = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(render_group, std::make_unique<Systems::CameraSystem>());
			ecs->get_system_manager().add_to_group(render_group, std::make_unique<Systems::Exit>());
			ecs->get_system_manager().add_to_group(render_group, std::make_unique<systems::menu_system>());
			ecs->get_system_manager().add_to_group(render_group, std::make_unique<Systems::GraphicsInterface>(
				Systems::GraphicsInterface(&graphics)));
		}
	}
}