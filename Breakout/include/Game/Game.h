#pragma once
#include "ECSConfig.h"
#include "EventConfig.h"
#include "World.h"
#include "Scores.h"

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
	game::scores scores;

	game::world	world;
	std::reference_wrapper<events::subscriber<events::switch_world>> world_listener;
	std::reference_wrapper<events::subscriber<events::new_score>> score_listener;
};
