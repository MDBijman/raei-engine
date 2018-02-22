#pragma once
#include "Game/ECSConfig.h"

namespace components
{
	struct score : public ecs::Component
	{
		int count;
	};
}