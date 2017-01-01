#pragma once
#include "Modules/Graphics/Graphics.h"
#include "Modules/IO/Input.h"
#include "Modules/ECS/ECS.h"
#include "Modules/Time/Timer.h"
#include "GameGraphics.h"
#include "GameConfig.h"
#include "Components/Components.h"
#include "Systems/Systems.h"
#include "Modules/Importers/Obj.h"

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window) :
		ecs(),
		graphics(hInstance, window),
		gameState(PAUSED)
	{
		//glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.f, 100.0f)


		auto cameraEntity = ecs.createEntity();
		auto& camera = ecs.addComponent(cameraEntity, Components::Camera2D(
			Graphics::PerspectiveCamera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(), 90.0f, 1280.f / 720.f, 0.1f, 100.0f)
		));
		
		auto sprite = ecs.createEntity();
		{
			ecs.addComponent(sprite, Components::Position2D());
			ecs.addComponent(sprite, Components::Velocity2D());

			auto spriteData = Components::SpriteAttributes{
				{
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
					}
				},
				{ 0, 1, 2, 2, 1, 3 }
			};

			auto uniform = Components::SpriteUniforms{ *graphics.renderer->context, {
				Graphics::Data::UniformBuffer<glm::mat4, 0, vk::ShaderStageFlagBits::eVertex> {
					camera.camera.getMatrices().projection * camera.camera.getMatrices().view * glm::mat4(), *graphics.renderer->context
				},
				Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment> {
					"./res/textures/potion.dds", vk::Format::eBc3UnormBlock, graphics.renderer->context->physicalDevice, graphics.renderer->context->device, graphics.renderer->cmdPool, *graphics.renderer->queue
				}
			} };

			auto& shader = ecs.addComponent(sprite, Components::SpriteShader{ std::move(spriteData), std::move(uniform) });
			shader.allocate(*graphics.renderer->context);
			
			auto& pipeline = ecs.addComponent(sprite, Components::Pipeline{ "./res/shaders/sprite-pipeline.json", graphics.renderer->renderPass, graphics.renderer->pipelineCache, graphics.renderer->context->device, shader });
			ecs.addComponent(sprite, Components::CommandBuffers{ graphics.renderer->cmdPool, *graphics.renderer->swapchain, graphics.renderer->context->device, glm::vec2{1280, 720}, graphics.renderer->renderPass, pipeline.pipeline, graphics.renderer->frameBuffers, shader });
		}


		{
			ecs.addSystem<Systems::Movement2D>();
			ecs.addSystem<Systems::Exit>();
			ecs.addSystem<Systems::Input>();
			ecs.addSystem<Systems::SpriteShaderSystem>(*graphics.renderer->context);
			ecs.addSystem<Systems::GraphicsInterface>(&graphics);
		}
	}

	void run()
	{
		gameState = RUNNING;

		Time::Timer t;
		while(true)
		{
			t.start();

			/////

			IO::Polling::update();
			ecs.updateSystems(t.dt());

			////

			t.end();
		}

		// gameState = GameState::FINISHED;
	}

private:
	MyECSManager   ecs;
	GameGraphics   graphics;

	enum GameState
	{
		PAUSED,
		RUNNING,
		FINISHED
	} gameState;

	float rotationSpeed = 0.3f;
	double dt = 0.0;
};
