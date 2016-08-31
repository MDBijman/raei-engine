#pragma once
#include "Modules\ECS\ECS.h"
#include "Modules\IO\Input.h"
#include "Game\GameConfig.h"

#include <windows.h>
#include <iostream>

namespace Systems
{
	class Exit : public MySystem
	{
	public:
		virtual void update(MyECSManager& ecs, double dt)
		{
			auto status = IO::Keyboard::getKeyStatus(VK_ESCAPE);
			if (status == IO::Keyboard::KeyStatus::DOWN)
			{
				exit(0);
			}
		}
	};
}
