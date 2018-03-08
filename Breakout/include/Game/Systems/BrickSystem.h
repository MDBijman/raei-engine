#pragma once
#include "Game/EventConfig.h"
#include "Game/ECSConfig.h"

namespace systems
{
	class brick_system : public MySystem
	{
		events::subscriber<events::collision>& subscriber;

	public:
		brick_system(events::subscriber<events::collision>& sub) : subscriber(sub) {}

		void update(ecs_manager& ecs) override;
	};
}