#pragma once
#include "ECSConfig.h"
#include "Modules/Graphics/Logic/Renderer.h"

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window);

	void run();

private:

	ecs_manager ecs;
	Graphics::Renderer	graphics;
	Graphics::Camera	camera;

	enum GameState
	{
		PAUSED,
		RUNNING,
		FINISHED
	} gameState;
};
