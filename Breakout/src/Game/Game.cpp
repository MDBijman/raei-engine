#include "stdafx.h"
#include "Game/Game.h"
#include "Game/Worlds/MainLevel.h"
#include "Game/Worlds/Menu.h"
#include "Game/Worlds/ScoreList.h"

// Modules
#include "Modules/IO/Input.h"
#include "Modules/ECS/ECSManager.h"
#include "Modules/Time/Timer.h"
#include "Modules/Importers/Obj.h"
#include "Modules/Events/EventManager.h"

#include <memory>

Game::Game(HINSTANCE hInstance, HWND window) :
	graphics({ hInstance, window, "triangle", 1280, 720 }),
	camera(-1272.f / 689.f, 1272.f / 689.f, -1.0f, 1.0f, 0.1f, 100.0f),
	asset_manager("./res/"),
	world(game::load_menu(camera, graphics)),
	world_listener(world.events.new_subscriber<events::switch_world>())
{}

void Game::run()
{
	while (true)
	{
		IO::Polling::update();
		world.update();

		while (world_listener.get().size() > 1)
		{
			auto event = world_listener.get().pop();
			if (event->to == game::worlds::QUIT)
			{
				return;
			}
			else if (event->to == game::worlds::GAME)
			{
				world = game::load(camera, graphics);
				world_listener = world.events.new_subscriber<events::switch_world>();
			}
			else if (event->to == game::worlds::MAIN_MENU)
			{
				world = game::load_menu(camera, graphics);
				world_listener = world.events.new_subscriber<events::switch_world>();
			}
			else if (event->to == game::worlds::HIGHSCORES)
			{
				world = game::load_scorelist(camera, graphics);
				world_listener = world.events.new_subscriber<events::switch_world>();
			}
		}
	}
}
