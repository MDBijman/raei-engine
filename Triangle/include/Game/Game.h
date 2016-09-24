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

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window) :
		assets(),
		ecs(),
		graphics(hInstance, window),
		gameState(GameState::PAUSED)
	{
		auto cameraEntity = ecs.createEntity();
		auto& camera = ecs.createComponent<Components::Camera2D>(cameraEntity, glm::vec2(1280, 720), 60.0f, 0.1f, 256.f);

		auto e = ecs.createEntity();
		{
			ecs.createComponent<Components::Position2D>(e);
			ecs.createComponent<Components::Velocity2D>(e);
			ecs.createComponent<Components::Input>(e);
			auto& texture = ecs.createComponent<Components::Texture>(e, "tree.dds", *graphics.renderer->device, *graphics.renderer->physicalDevice, graphics.renderer->cmdPool, *graphics.renderer->queue);

			auto& m = ecs.createComponent<Components::Mesh>(e, "bad_car.obj", *graphics.renderer->device, *graphics.renderer->physicalDevice);

			auto& ms = ecs.createComponent<Components::MeshShader>(e, *graphics.renderer->device, *graphics.renderer->physicalDevice);
			auto& p = ecs.createComponent<Components::Pipeline>(e, "./res/shaders/default-pipeline.json", graphics.renderer->renderPass, graphics.renderer->pipelineCache, *graphics.renderer->device, m.mesh.vertices.vi, texture.texture, ms);
			ecs.createComponent<Components::CameraID>(e, uint32_t(1));

			ecs.createComponent<Components::CommandBuffers>(e, graphics.renderer->cmdPool, *graphics.renderer->swapchain, *graphics.renderer->device, camera, graphics.renderer->renderPass, p.pipeline, graphics.renderer->frameBuffers, ms, m.mesh);
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
		gameState = GameState::RUNNING;

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
