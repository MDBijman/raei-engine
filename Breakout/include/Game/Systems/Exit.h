#pragma once
#include "Game/ECSConfig.h"

namespace Systems
{
	class Exit : public MySystem
	{
	public:
		void update(ecs_manager& ecs) override;
	};
}
