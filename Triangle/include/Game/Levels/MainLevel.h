#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"
#include "Game/Systems/Systems.h"
#include "Game/Events/BrickEvents.h"

#include "Modules/Graphics/Core/Camera.h"

namespace game
{
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

			auto spriteData = Components::sprite_attributes({
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

			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });

			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& shader = ecs->addComponent(sprite, Components::sprite_shader(std::move(spriteData),
				std::move(indices), std::move(uniform)));
			shader.allocate(*graphics.context);

			auto& pipeline = ecs->addComponent(sprite, Components::Pipeline{
				"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
				graphics.context->device, shader
				});

			ecs->addComponent(sprite, Components::CommandBuffers{
				graphics.context->cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2(1272, 689),
				graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
				});
		}

		auto ball = ecs->createEntity();
		{
			ecs->addComponent(ball, Components::Position2D(0.0, 0.7));
			ecs->addComponent(ball, Components::Velocity2D(0.0, 1.6));
			ecs->addComponent(ball, Components::Scale2D(.03, .03));

			auto spriteData = Components::sprite_attributes({
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

			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });

			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& shader = ecs->addComponent(ball, Components::sprite_shader(std::move(spriteData),
				std::move(indices), std::move(uniform)));
			shader.allocate(*graphics.context);

			auto& pipeline = ecs->addComponent(ball, Components::Pipeline{
				"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
				graphics.context->device, shader
				});

			ecs->addComponent(ball, Components::CommandBuffers{
				graphics.context->cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2(1272, 689),
				graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
				});
		}

		std::unordered_set<uint32_t> bricks;

		// Bricks
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				auto block = ecs->createEntity();
				bricks.insert(block);

				float width = 0.135f;
				float gap = 0.05f;

				auto& pos = ecs->addComponent(block, Components::Position2D(
					-0.9 + (width + gap) * i + 0.5f * width,
					-0.9 + (double)j / 8.0
				));
				auto& scale = ecs->addComponent(block, Components::Scale2D(width, 1.0f / 15.0f));

				auto spriteData = Components::sprite_attributes({
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

				auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });

				auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
					});

				auto& shader = ecs->addComponent(block, Components::sprite_shader{
					std::move(spriteData), std::move(indices), std::move(uniform)
					});

				shader.allocate(*graphics.context);

				auto& pipeline = ecs->addComponent(block, Components::Pipeline{
					"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
					graphics.context->device, shader
					});

				ecs->addComponent(block, Components::CommandBuffers{
					graphics.context->cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2{ 1272, 689 },
					graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
					});

			}
		}

		// Top Wall
		{
			auto top_wall = ecs->createEntity();

			auto& pos = ecs->addComponent(top_wall, Components::Position2D(0.0, -1.0));
			auto& scale = ecs->addComponent(top_wall, Components::Scale2D(2.0, 1.0 / 15.0));

			auto spriteData = Components::sprite_attributes({
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

			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });

			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& shader = ecs->addComponent(top_wall, Components::sprite_shader{
				std::move(spriteData), std::move(indices), std::move(uniform)
				});

			shader.allocate(*graphics.context);

			auto& pipeline = ecs->addComponent(top_wall, Components::Pipeline{
				"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
				graphics.context->device, shader
				});

			ecs->addComponent(top_wall, Components::CommandBuffers{
				graphics.context->cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2{ 1272, 689 },
				graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
				});
		}

		// Left Wall
		{
			auto left_wall = ecs->createEntity();

			auto& pos = ecs->addComponent(left_wall, Components::Position2D(-1.0, 0.0));
			auto& scale = ecs->addComponent(left_wall, Components::Scale2D(1.0 / 15.0, 2.0));

			auto spriteData = Components::sprite_attributes({
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

			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });

			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& shader = ecs->addComponent(left_wall, Components::sprite_shader{
				std::move(spriteData), std::move(indices), std::move(uniform)
				});

			shader.allocate(*graphics.context);

			auto& pipeline = ecs->addComponent(left_wall, Components::Pipeline{
				"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
				graphics.context->device, shader
				});

			ecs->addComponent(left_wall, Components::CommandBuffers{
				graphics.context->cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2{ 1272, 689 },
				graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
				});
		}

		// Right Wall
		{
			auto left_wall = ecs->createEntity();

			auto& pos = ecs->addComponent(left_wall, Components::Position2D(1.0, 0.0));
			auto& scale = ecs->addComponent(left_wall, Components::Scale2D(1.0 / 15.0, 2.0));

			auto spriteData = Components::sprite_attributes({
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

			auto indices = Components::sprite_indices({ 0, 1, 2, 2, 1, 3 });

			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& shader = ecs->addComponent(left_wall, Components::sprite_shader(
				std::move(spriteData), std::move(indices), std::move(uniform)
			));

			shader.allocate(*graphics.context);

			auto& pipeline = ecs->addComponent(left_wall, Components::Pipeline{
				"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
				graphics.context->device, shader
				});

			ecs->addComponent(left_wall, Components::CommandBuffers{
				graphics.context->cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2{ 1272, 689 },
				graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
				});
		}

		// Score
		{
			auto score = ecs->createEntity();

			ecs->addComponent<components::score>(score, components::score());

			auto& pos = ecs->addComponent(score, Components::Position2D(-1.8361, -.99));
			auto& scale = ecs->addComponent(score, Components::Scale2D(.3, .1));

			auto uniform = Components::sprite_uniforms(*graphics.context, {
				graphics.resources.create_buffer<glm::mat4>(
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 0),
				graphics.resources.create_texture("./res/fonts/vcr_osd_mono.dds", vk::Format::eBc3UnormBlock, 1,
					vk::ShaderStageFlagBits::eFragment)
				});

			auto& shader = ecs->addComponent(score, Components::sprite_shader(
				Components::sprite_attributes({
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
					}),
				Components::sprite_indices({
					// Quad 1
					0, 1, 2, 2, 1, 3,
					// Quad 2 
					4, 5, 6, 6, 5, 7,
					// Quad 3
					8, 9, 10, 10, 9, 11,
					}),
					std::move(uniform)
					));

			shader.allocate(*graphics.context);

			auto& pipeline = ecs->addComponent(score, Components::Pipeline{
				"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
				graphics.context->device, shader
				});

			ecs->addComponent(score, Components::CommandBuffers{
				graphics.context->cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2(1272, 689),
				graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
				});
		}

		{
			auto sg = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(sg, std::make_unique<Systems::Exit>());
			ecs->get_system_manager().add_to_group(sg, std::make_unique<Systems::Input>());
			ecs->get_system_manager().add_to_group(sg, std::make_unique<Systems::CameraSystem>());
			ecs->get_system_manager().add_to_group(sg, std::make_unique<systems::game_rule_system>(ball));
			ecs->get_system_manager().add_to_group(sg, std::make_unique<systems::fps_system>());

			auto render_thread = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(render_thread, std::make_unique<Systems::GraphicsInterface>(&graphics));
			ecs->get_system_manager().add_to_group(render_thread, std::make_unique<Systems::SpriteShaderSystem>(*graphics.context));

			auto pt = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(pt, std::make_unique<Systems::Movement2D>());
			ecs->get_system_manager().add_to_group(pt,
				std::make_unique<systems::physics_system>(events->new_publisher<events::brick_hit>(), ball, sprite, bricks));

			auto event_handler_group = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(event_handler_group,
				std::make_unique<systems::brick_event_system>(events->new_subscriber<events::brick_hit>()));
			ecs->get_system_manager().add_to_group(event_handler_group,
				std::make_unique<systems::score_system>(events->new_subscriber<events::brick_hit>(), *graphics.context));
		}
	}
}