#pragma once
#include "Game/ECSConfig.h"

namespace Systems
{
	class CameraSystem : public MySystem
	{
	public:
		void update(ecs_manager& ecs) override;
	};
}