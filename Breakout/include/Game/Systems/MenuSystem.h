#pragma once
#include "Game/EventConfig.h"
#include "Game/ECSConfig.h"
#include "Game/Events/WorldEvents.h"
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

		void update(ecs_manager& ecs) override;
	};
}
