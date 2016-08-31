#pragma once
#include "Modules\Graphics\Graphics.h"
#include "Modules\IO\Input.h"
#include "Modules\ECS\ECS.h"
#include "Modules\AssetManager\AssetManager.h"
#include "Modules\Time\Timer.h"
#include "GameGraphics.h"
#include "GameConfig.h"
#include "Components\Components.h"
#include "Systems\Systems.h"

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window) :
		graphics(hInstance, window),
		assets(),
		ecs(),
		gameState(GameState::PAUSED)
	{
		auto e = ecs.createEntity();
		ecs.createComponent<Components::Position2D>(e);
		ecs.createComponent<Components::Velocity2D>(e);
		ecs.createComponent<Components::Input>(e);
		ecs.createComponent<Components::Texture>(e);

		ecs.addSystem<Systems::Movement2D>();
		ecs.addSystem<Systems::Exit>();
		ecs.addSystem<Systems::Input>();
		ecs.addSystem<Systems::GraphicsInterface>();
	}

	void run()
	{
		gameState = GameState::RUNNING;

		Time::Timer t;
		while (true)
		{
			t.start();

			/////

			IO::Polling::update();
			ecs.updateSystems(t.dt());
			graphics.render();

			////

			t.end();
		}

		gameState = GameState::FINISHED;
	}

private:
	MyAssetManager assets;
	MyECSManager ecs;
	GameGraphics graphics;

	enum GameState
	{
		PAUSED,
		RUNNING,
		FINISHED
	} gameState;

	float rotationSpeed = 0.3f;
	double dt = 0.0;
};
