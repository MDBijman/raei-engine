#include "stdafx.h"
#include "Game/Systems/MenuSystem.h"

#include <windows.h>

#include "Game/WorldConfig.h"
#include "Modules/IO/Input.h"

void systems::menu_system::update(ecs_manager& ecs)
{
	auto& pointers = ecs.filterEntities<ecs::filter<components::pointer>>();
	assert(pointers.entities.size() == 1);
	auto pointer = pointers.entities.begin();
	auto& pos = ecs.getComponent<Components::Position2D>(*pointer).pos;

	auto down_key_status = IO::Keyboard::getKeyStatus(VK_DOWN);
	auto up_key_status = IO::Keyboard::getKeyStatus(VK_UP);

	if (timer.dt() > 0.2f)
	{
		if (down_key_status == IO::Keyboard::DOWN)
		{
			timer.zero();
			// Play
			if (pos.y == -0.2f)
			{
				pos.y = 0.0f;
			}
			// Scores
			else if (pos.y == 0.0f)
			{
				pos.y = 0.2f;
			}
			// Quit
			else if (pos.y == 0.2f)
			{
				pos.y = -0.2f;
			}
		}
		else if (up_key_status == IO::Keyboard::DOWN)
		{
			timer.zero();
			// Play
			if (pos.y == -0.2f)
			{
				pos.y = 0.2f;
			}
			// Scores
			else if (pos.y == 0.0f)
			{
				pos.y = -0.2f;
			}
			// Quit
			else if (pos.y == 0.2f)
			{
				pos.y = 0.0f;
			}
		}
	}

	auto enter_key_status = IO::Keyboard::getKeyStatus(VK_RETURN);
	if (enter_key_status == IO::Keyboard::KeyStatus::DOWN)
	{
		if (pos.y == -0.2f)
		{
			switch_publisher.broadcast(events::switch_world(game::worlds::GAME));
		}
		else if (pos.y == 0.0f)
		{
			switch_publisher.broadcast(events::switch_world(game::worlds::HIGHSCORES));
		}
		else if (pos.y == 0.2f)
		{
			switch_publisher.broadcast(events::switch_world(game::worlds::QUIT));
		}
	}
}