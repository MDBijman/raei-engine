#pragma once
#include "Modules/Graphics/Graphics.h"
#include "Modules/IO/Input.h"
#include "Modules/ECS/ECS.h"
#include "Modules/AssetManager/AssetManager.h"
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
		assets(),
		ecs(),
		graphics(hInstance, window),
		gameState(PAUSED)
	{
		auto cameraEntity = ecs.createEntity();
		auto& camera = ecs.addComponent(cameraEntity, Components::Camera2D{ glm::vec2(1280, 720), 60.0f, 0.1f, 256.f });

/* This is a mesh entity
		auto e = ecs.createEntity();
		{
			ecs.addComponent(e, Components::Position2D());
			ecs.addComponent(e, Components::Velocity2D());
			ecs.addComponent(e, Components::Input());

			auto& texture = ecs.addComponent(e, Components::Texture{ "tree.dds", graphics.renderer->context->device, graphics.renderer->context->physicalDevice, graphics.renderer->cmdPool, *graphics.renderer->queue });
			auto& m = ecs.addComponent(e, Components::Mesh{ "bad_car.obj", graphics.renderer->context->device, graphics.renderer->context->physicalDevice });
			auto& ms = ecs.addComponent(e, Components::MeshShader{ *graphics.renderer->context, texture.texture });
			auto& p = ecs.addComponent(e, Components::Pipeline{ "./res/shaders/default-pipeline.json", graphics.renderer->renderPass, graphics.renderer->pipelineCache, graphics.renderer->context->device, m.mesh->vertices.vi, texture.texture, ms });
			ecs.addComponent(e, Components::CameraID{ uint32_t(1) });
			ecs.addComponent(e, Components::CommandBuffers{ graphics.renderer->cmdPool, *graphics.renderer->swapchain, graphics.renderer->context->device, camera, graphics.renderer->renderPass, p.pipeline, graphics.renderer->frameBuffers, ms, m.mesh });
		}
*/
		
		auto sprite = ecs.createEntity();
		{
			ecs.addComponent(sprite, Components::Position2D());
			ecs.addComponent(sprite, Components::Velocity2D());

			auto spriteData = Components::AttributeType({
				{	
					{ 0.0f, 0.0f },
					{ -1.0f, 0.0f }
				},
				{ 
					{ 0.0f, 1.0f },
					{ -1.0f, 1.0f }
				},
				{ 
					{ 1.0f, 0.0f },
					{ 0.0f, 0.0f }
				}
			});

			auto uniform = Components::UniformType(*graphics.renderer->context, {
				Graphics::Data::Texture<1, vk::ShaderStageFlagBits::eFragment> {
					"./res/textures/potion.dds", vk::Format::eBc3UnormBlock, graphics.renderer->context->physicalDevice, graphics.renderer->context->device, graphics.renderer->cmdPool, *graphics.renderer->queue
				}
			});

			//auto uniform = Graphics::Data::Uniforms<>(*graphics.renderer->context, std::tuple<>());

			//auto& texture = ecs.addComponent(sprite, Components::Texture{"potion.dds", *graphics.renderer->context, graphics.renderer->cmdPool, *graphics.renderer->queue });
			auto& shader = ecs.addComponent(sprite, Components::SpriteShader{ *graphics.renderer->context, std::move(spriteData), std::move(uniform) });
			shader.upload(graphics.renderer->context->device, graphics.renderer->context->physicalDevice);

			auto& pipeline = ecs.addComponent(sprite, Components::Pipeline{ "./res/shaders/sprite-pipeline.json", graphics.renderer->renderPass, graphics.renderer->pipelineCache, graphics.renderer->context->device, shader });
			ecs.addComponent(sprite, Components::CommandBuffers{ graphics.renderer->cmdPool, *graphics.renderer->swapchain, graphics.renderer->context->device, camera, graphics.renderer->renderPass, pipeline.pipeline, graphics.renderer->frameBuffers, shader });
		}


		{
			ecs.addSystem<Systems::Movement2D>();
			ecs.addSystem<Systems::Exit>();
			ecs.addSystem<Systems::Input>();
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
	MyAssetManager assets;
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
