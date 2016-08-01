#pragma once
#include "Modules\Graphics\Logic\Camera.h"
#include "Modules\Graphics\Logic\Drawable.h"
#include "Modules\Graphics\Logic\Renderer.h"
#include "Modules\Input\Input.h"
#include "ecs\ECS.h"
#include "GameGraphics.h"
#include "AssetManager.h"
#include "InputPoller.h"

#include "Systems\MovementSystem.h"

#include <chrono>
#include <windows.h>
#include <iostream>

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
		ecs.addComponent<Position3D>(e);

		ecs.addSystem<MovementSystem>();
	}

	void run()
	{
		gameState = GameState::RUNNING;

		while (true)
		{
			auto startTime = std::chrono::high_resolution_clock::now();

			// Escape button
			if (Input::Keyboard::getKeyStatus(VK_ESCAPE) == Input::Keyboard::KeyStatus::DOWN)
				exit(0);
			// Move forward or backward
			if (Input::Keyboard::getKeyStatus('W') == Input::Keyboard::KeyStatus::DOWN)
				graphics.camera.forward(3.0f, dt);
			if (Input::Keyboard::getKeyStatus('S') == Input::Keyboard::KeyStatus::DOWN)
				graphics.camera.backward(3.0f, dt);
			// Move left or right
			if (Input::Keyboard::getKeyStatus('A') == Input::Keyboard::KeyStatus::DOWN)
				graphics.camera.left(3.0f, dt);
			if (Input::Keyboard::getKeyStatus('D') == Input::Keyboard::KeyStatus::DOWN)
				graphics.camera.right(3.0f, dt);

			graphics.camera.rotate(Input::Mouse::getDX() * rotationSpeed * 20.0f, -Input::Mouse::getDY() * rotationSpeed * 20.0f, dt);

			/////

			input.update();
			ecs.updateSystems(dt);
			graphics.render();

			////

			auto endTime = std::chrono::high_resolution_clock::now();
			auto difference = std::chrono::duration<double, std::milli>(endTime - startTime).count();
			dt = difference / 1000;
		}

		gameState = GameState::FINISHED;
	}

private:
	InputPoller input;
	AssetManager assets;
	GameGraphics graphics;

	using CList = ComponentList<
		Position3D
	>;

	using FList = FilterList <
		Filter<Position3D>
	>;

	ECSManager<CList, FList> ecs;

	enum GameState
	{
		PAUSED,
		RUNNING,
		FINISHED
	} gameState;

	float rotationSpeed = 0.3f;
	double dt = 0.0;
};
