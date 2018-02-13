#pragma once
#include "Modules/Events/EventManager.h"
#include "Game/Systems/physics_system.h"

using event_manager = events::base_manager<
	events::base_subscriber<systems::physics_system>
>;