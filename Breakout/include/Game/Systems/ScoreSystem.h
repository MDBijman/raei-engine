#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"

namespace systems
{
	class score_system : public MySystem
	{
		events::subscriber<events::collision>& subscriber;

	public:
		score_system(events::subscriber<events::collision>& sub) : 
			 subscriber(sub)
		{}

		void update(ecs_manager& ecs) override;
	};
}
