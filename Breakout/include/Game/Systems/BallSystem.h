#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"

namespace systems
{
	class ball_system : public MySystem
	{
		events::subscriber<events::collision>& subscriber;
	public:
		ball_system(events::subscriber<events::collision>& sub) : subscriber(sub) {}

		void update(ecs_manager& ecs) override;

		void bounce(ecs_manager& ecs, uint32_t block, uint32_t ball);
	};
}