#pragma once
#include "Game/ECSConfig.h"
#include "Modules/Time/Timer.h"

namespace Systems
{
	class Input : public MySystem
	{
		Time::Timer2 timer;

	public:
		Input()
		{
			timer.zero();
		}

		void update(ecs_manager& ecs) override;
	};
}
