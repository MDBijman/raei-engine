#pragma once
#include "Modules\Graphics\Graphics.h"
#include "Modules\IO\Input.h"
#include "Modules\ECS\ECS.h"
#include "Modules\AssetManager\AssetManager.h"
#include "Modules\Time\Timer.h"
#include "Modules\Lisp\AST.h"

#include "GameGraphics.h"
#include "GameConfig.h"
#include "Systems\MovementSystem.h"
#include "Systems\ExitSystem.h"
#include "Systems\InputSystem.h"

#include <chrono>
#include <fstream>

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
		ecs.createComponent<Components::Position3D>(e);
		ecs.createComponent<Components::Velocity3D>(e, 1.0f, 0.0f, 3.0f);
		ecs.createComponent<Components::Orientation3D>(e);
		ecs.createComponent<Components::Input>(e);

		ecs.addSystem<Systems::MovementSystem>();
		ecs.addSystem<Systems::ExitSystem>();
		ecs.addSystem<Systems::InputSystem>();

		
	}

	void run()
	{
		gameState = GameState::RUNNING;

		Time::Timer t;
		while (true)
		{
			t.start();

			/////

			Input::Polling::update();
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
