#pragma once
#include <windows.h>
#include "../ECSConfig.h"
#include "Modules/IO/Input.h"
#include "Modules/Time/Timer.h"

namespace systems
{
	class menu_system : public MySystem
	{
		Time::Timer2 timer;

	public:
		menu_system()
		{
			timer.zero();
		}

		void update(ecs_manager& ecs) override
		{
			auto& pointers = ecs.filterEntities<ecs::filter<components::pointer>>();
			assert(pointers.entities.size() == 1);
			auto pointer = pointers.entities.begin();
			auto& pos = ecs.getComponent<Components::Position2D>(*pointer).pos;

			auto key_status = IO::Keyboard::getKeyStatus(VK_DOWN);

			if (timer.dt() > 0.1f && key_status == IO::Keyboard::DOWN)
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
		}
	};
}
