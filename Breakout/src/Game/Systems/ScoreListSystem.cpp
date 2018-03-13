#include "stdafx.h"
#include "Game/Systems/ScoreListSystem.h"
#include "Modules/IO/Input.h"
#include <Windows.h>

namespace systems
{
	score_list_system::score_list_system(events::publisher<events::switch_world>& pub) :
		publisher(pub)
	{

	}

	void score_list_system::update(ecs_manager& ecs)
	{
		if (IO::Keyboard::getKeyStatus(VK_RETURN) == IO::Keyboard::DOWN)
		{
			publisher.broadcast(events::switch_world(game::worlds::MAIN_MENU));
		}
	}
}