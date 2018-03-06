#pragma once
#include "Modules/Events/EventManager.h"
#include "Game/Events/BrickEvents.h"
#include "Game/Events/WorldEvents.h"

using event_manager = events::base_manager<
	events::collision,
	events::switch_world
>;