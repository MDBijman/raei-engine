#pragma once
#include <vector>

#include "Game/ECSConfig.h"
#include "Modules/Time/Timer.h"

namespace Systems
{
	class Movement2D : public MySystem
	{
		Time::Timer2 timer;

	public:
		Movement2D()
		{
			timer.zero();
		}

		void update(ecs_manager& ecs) override;
	};
}
