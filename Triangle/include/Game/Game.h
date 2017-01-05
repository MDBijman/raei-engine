#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Modules/Graphics/Graphics.h"
#include "Modules/IO/Input.h"
#include "Modules/ECS/ECS.h"
#include "Modules/Time/Timer.h"
#include "GameGraphics.h"
#include "GameConfig.h"
#include "Components/Components.h"
#include "Systems/Systems.h"
#include "Modules/Importers/Obj.h"

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>
#include <string>

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window) :
		ecs(),
		graphics(hInstance, window),
		gameState(PAUSED)
	{
		chaiscript::ChaiScript chai(chaiscript::Std_Lib::library());
		chai.add(chaiscript::fun([](const std::string& name) -> std::string {
			return "Hello " + name + "!";
		}), "helloWorld");

		chai.eval(R"(
			puts(helloWorld("Bob"));
		)");



		auto cameraEntity = ecs.createEntity();
		auto& pos = ecs.addComponent(cameraEntity, Components::Position3D(0.0f, 0.0f, 1.0f));
		ecs.addComponent(cameraEntity, Components::Orientation3D());
		auto& camera = ecs.addComponent(cameraEntity, Components::Camera2D(Graphics::Camera(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f)));
		camera.camera.moveTo(pos.pos, glm::vec3());
		
		auto sprite = ecs.createEntity();
		{
			ecs.addComponent(sprite, Components::Position2D());
			ecs.addComponent(sprite, Components::Velocity2D());
			ecs.addComponent(sprite, Components::Input(0.1f));

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
			ecs.addSystem<Systems::CameraSystem>();
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
			ecs.updateSystems(t.dt() / 1000.0);

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

	double dt = 0.0;
};
