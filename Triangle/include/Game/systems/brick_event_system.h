#pragma once
#include "Game/EventConfig.h"
#include "Game/Events/BrickEvents.h"
#include "Game/ECSConfig.h"

namespace systems
{
	class brick_event_system : public MySystem
	{
	public:
		brick_event_system(events::subscriber<events::brick_hit>& sub) : subscriber(sub) {}

		void update(ecs_manager& ecs) override
		{
			while (subscriber.size() > 0)
			{
				auto hit = subscriber.pop();
				ecs.removeEntity(hit->brick_id);
			}
		}

	private:
		events::subscriber<events::brick_hit>& subscriber;
	};
}