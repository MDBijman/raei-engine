#pragma once
#include "ECSConfig.h"
#include "EventConfig.h"
#include "World.h"

#include "Events/WorldEvents.h"

#include "Modules/Graphics/Core/Renderer.h"
#include "Modules/Assets/AssetManager.h"

class Game
{
public:
	Game(HINSTANCE hInstance, HWND window);

	void run();

private:
	graphics::Renderer graphics;
	graphics::Camera camera;
	assets::manager	asset_manager;

	game::world	world;
	events::subscriber<events::switch_world>& world_listener;
};
