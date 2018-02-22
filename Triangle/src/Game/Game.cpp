#include "stdafx.h"
#include "Game/Game.h"
#include "Game/Levels/MainLevel.h"

// Modules
#include "Modules/IO/Input.h"
#include "Modules/ECS/ECSManager.h"
#include "Modules/Time/Timer.h"
#include "Modules/Importers/Obj.h"
#include "Modules/Events/EventManager.h"

#include <memory>

Game::Game(HINSTANCE hInstance, HWND window) :
	ecs(std::make_shared<ecs_manager>()),
	events(std::make_shared<event_manager>()),
	graphics({ hInstance, window, "triangle", 1280, 720 }),
	gameState(PAUSED),
	camera(-1272.f / 689.f, 1272.f / 689.f, -1.0f, 1.0f, 0.1f, 100.0f)
{
	game::load(ecs, events, camera, graphics);
}

void Game::run()
{
	gameState = RUNNING;

	while (true)
	{
		IO::Polling::update();
		ecs->update();
	}

	gameState = GameState::FINISHED;
}
