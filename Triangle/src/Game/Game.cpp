#include "stdafx.h"
#include "Game/Game.h"

// Game
#include "Game/Systems/Systems.h"

// Modules
#include "Modules/IO/Input.h"
#include "Modules/ECS/ECSManager.h"
#include "Modules/Time/Timer.h"
#include "Modules/Importers/Obj.h"
//#include "Modules/ECSFactory/ECSFactory.h"
//#include "Modules/LuaECS/LuaECS.h"
//#include <filesystem>

Game::Game(HINSTANCE hInstance, HWND window) :
	ecs(),
	graphics({
		hInstance, window, "triangle", 1280, 720
		}),
	gameState(PAUSED),
	camera(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f)
{
	auto cameraEntity = ecs.createEntity();
	auto& pos = ecs.addComponent(cameraEntity, Components::Position3D(0.0f, 0.0f, 1.0f));
	ecs.addComponent(cameraEntity, Components::Orientation3D());
	auto& camera = ecs.addComponent(cameraEntity, Components::Camera2D(this->camera));
	camera.camera.moveTo(pos.pos, glm::vec3());

	auto sprite = ecs.createEntity();
	{
		ecs.addComponent(sprite, Components::Position2D(glm::vec2(0.0, 0.8)));
		ecs.addComponent(sprite, Components::Velocity2D(glm::vec2(0.0, 0.0)));
		ecs.addComponent(sprite, Components::Scale2D(glm::vec2{ .2f, 0.05f }));
		ecs.addComponent(sprite, Components::Input(1.0f));

		auto spriteData = Components::SpriteAttributes{
			{
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
			},
			{ 0, 1, 2, 2, 1, 3 }
		};

		auto uniform = Components::SpriteUniforms(*graphics.context, {
				Graphics::Data::UniformBuffer<glm::mat4, 0, vk::ShaderStageFlagBits::eVertex> {
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 
					*graphics.context
				},
				Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment> {
					"./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, graphics.context->physicalDevice, 
					graphics.context->device, graphics.cmdPool, *graphics.queue
				}
			});

		auto& shader = ecs.addComponent(sprite, Components::SpriteShader{ std::move(spriteData), std::move(uniform) });
		shader.allocate(*graphics.context);

		auto& pipeline = ecs.addComponent(sprite, Components::Pipeline{
			"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
			graphics.context->device, shader
			});

		ecs.addComponent(sprite, Components::CommandBuffers{
			graphics.cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2{ 1280, 720 },
			graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
			});
	}

	auto ball = ecs.createEntity();
	{
		ecs.addComponent(ball, Components::Position2D(glm::vec2(0.0, 0.7)));
		ecs.addComponent(ball, Components::Velocity2D(glm::vec2(0.0, 0.005)));
		ecs.addComponent(ball, Components::Scale2D(glm::vec2{ .05f * (720.f / 1280.f), .05f }));

		auto ballData = Components::SpriteAttributes{
			{
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
			},
			{ 0, 1, 2, 2, 1, 3 }
		};

		auto uniform = Components::SpriteUniforms(*graphics.context, {
				Graphics::Data::UniformBuffer<glm::mat4, 0, vk::ShaderStageFlagBits::eVertex> {
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), 
					*graphics.context
				},
				Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment> {
					"./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, graphics.context->physicalDevice, 
					graphics.context->device, graphics.cmdPool, *graphics.queue
				}
			});

		auto& shader = ecs.addComponent(ball, Components::SpriteShader{ std::move(ballData), std::move(uniform) });
		shader.allocate(*graphics.context);

		auto& pipeline = ecs.addComponent(ball, Components::Pipeline{
			"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
			graphics.context->device, shader
			});

		ecs.addComponent(ball, Components::CommandBuffers{
			graphics.cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2{ 1280, 720 },
			graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
			});
	}

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			auto block = ecs.createEntity();

			float width = 0.135f;
			float gap = 0.05f;

			auto& pos = ecs.addComponent(block, Components::Position2D(glm::vec2(
				-0.9 + (width + gap) * i + 0.5f * width,
				-0.9 + (double)j / 8.0
			)));
			auto& scale = ecs.addComponent(block, Components::Scale2D(
				glm::vec2{ width, 1.0f / 15.0f }
			));

			auto spriteData = Components::SpriteAttributes{
				{
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
				},
				{ 0, 1, 2, 2, 1, 3 }
			};

			auto uniform = Components::SpriteUniforms(*graphics.context, {
				Graphics::Data::UniformBuffer<glm::mat4, 0, vk::ShaderStageFlagBits::eVertex> {
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(),
					*graphics.context
				},
				Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment> {
					"./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, graphics.context->physicalDevice,
					graphics.context->device, graphics.cmdPool, *graphics.queue
				}
				});

			auto& shader = ecs.addComponent(block, Components::SpriteShader{
				std::move(spriteData), std::move(uniform)
				});

			shader.allocate(*graphics.context);

			auto& pipeline = ecs.addComponent(block, Components::Pipeline{
				"./res/shaders/sprite-pipeline.json", graphics.drawPass, graphics.pipelineCache,
				graphics.context->device, shader
				});

			ecs.addComponent(block, Components::CommandBuffers{
				graphics.cmdPool, *graphics.swapchain, graphics.context->device, glm::vec2{ 1280, 720 },
				graphics.drawPass, pipeline.pipeline, graphics.frameBuffers, shader
				});

		}
	}

	{
		ecs.addSystem<Systems::Movement2D>();
		ecs.addSystem<Systems::Exit>();
		ecs.addSystem<Systems::Input>();
		ecs.addSystem<Systems::SpriteShaderSystem>(*graphics.context);
		ecs.addSystem<Systems::GraphicsInterface>(&graphics);
		ecs.addSystem<Systems::CameraSystem>();
	}
}

void Game::run()
{
	gameState = RUNNING;

	Time::Timer t;
	while (true)
	{
		t.start();

		/////

		IO::Polling::update();
		ecs.updateSystems(t.dt() / 1000.0);

		////

		t.stop();
	}

	// gameState = GameState::FINISHED;
}
