#pragma once
#include <windows.h>
#include "Game/ECSConfig.h"
#include "Game/WorldConfig.h"
#include "Game/Events/WorldEvents.h"
#include "Modules/IO/Input.h"
#include "Modules/Time/Timer.h"

namespace systems
{
	class menu_system : public MySystem
	{
		Time::Timer2 timer;
		events::publisher<events::switch_world>& switch_publisher;

	public:
		menu_system(events::publisher<events::switch_world>& switch_publisher) :
			switch_publisher(switch_publisher)
		{
			timer.zero();
		}

		void update(ecs_manager& ecs) override
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
				switch_publisher.broadcast(events::switch_world(game::worlds::QUIT));
			}
		}
	};
}
