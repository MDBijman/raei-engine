#pragma once
#include "ECSConfig.h"
#include "Modules/Graphics/Logic/Renderer.h"
#include "EventConfig.h"

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window);

	void run();

private:

	std::shared_ptr<ecs_manager> ecs;
	std::shared_ptr<event_manager> events;

	Graphics::Renderer	graphics;
	Graphics::Camera	camera;

	enum GameState
	{
		PAUSED,
		RUNNING,
		FINISHED
	} gameState;
};
