#pragma once
#include "GameConfig.h"
#include "Modules/Graphics/Logic/Renderer.h"

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window);

	void run();

private:
	MyECSManager		ecs;
	Graphics::Renderer	graphics;

	enum GameState
	{
		PAUSED,
		RUNNING,
		FINISHED
	} gameState;

	double dt = 0.0;
};
