#pragma once
#include "Game/ECSConfig.h"

#include "Modules/Time/Timer.h"

namespace systems
{
	class fps_system : public MySystem
	{
	public:
		fps_system() : timer() {}

		void update(ecs_manager& ecs, double dt) override
		{
			auto ms = timer.dt();
			std::cout << 1.0 / ms << "\n";
			timer.zero();
		}

	private:
		Time::Timer2 timer;
	};
}