#pragma once
#include "Game/ECSConfig.h"

namespace systems
{
	class game_rule_system : public MySystem
	{
	public:
		void update(ecs_manager& ecs) override;
	};
}