#pragma once
#include "Modules/ECS/System.h"
#include "Modules/IO/Input.h"
#include "../ECSConfig.h"

#include <windows.h>

namespace Systems
{
	class Exit : public MySystem
	{
	public:
		void update(ecs_manager& ecs) override
		{
			auto status = IO::Keyboard::getKeyStatus(VK_ESCAPE);
			if (status == IO::Keyboard::KeyStatus::DOWN)
			{
				exit(0);
			}
		}
	};
}
