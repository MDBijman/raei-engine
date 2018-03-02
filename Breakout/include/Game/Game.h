#pragma once
#include "ECSConfig.h"
#include "Modules/Graphics/Core/Renderer.h"
#include "Modules/Assets/AssetManager.h"
#include "EventConfig.h"

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window);

	void run();

private:

	std::shared_ptr<ecs_manager> ecs;
	std::shared_ptr<event_manager> events;

	graphics::Renderer	graphics;
	graphics::Camera	camera;
	assets::manager		asset_manager;

	enum GameState
	{
		PAUSED,
		RUNNING,
		FINISHED
	} gameState;
};
