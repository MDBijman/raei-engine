#pragma once
#include "Modules/ECS/Component.h"

namespace components
{
	struct brick : ecs::Component {};
	struct paddle : ecs::Component {};
	struct ball : ecs::Component {};
	struct wall : ecs::Component {};
	struct powerup : ecs::Component {};
}
