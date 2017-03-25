#pragma once
#include "Modules/ECS/System.h"
#include "Modules/IO/Input.h"
#include "Game/GameConfig.h"

#include <windows.h>

namespace Systems
{
	class Exit : public MySystem
	{
	public:
		void update(MyECSManager& ecs, double dt) override
		{
			auto status = IO::Keyboard::getKeyStatus(VK_ESCAPE);
			if (status == IO::Keyboard::KeyStatus::DOWN)
			{
				exit(0);
			}
		}
	};
}
