#pragma once
#include "Modules/ECS/ECSManager.h"
#include "Components/Camera2D.h"
#include "Components/CameraID.h"
#include "Components/Input.h"
#include "Components/Position2D.h"
#include "Components/Velocity2D.h"
#include "Components/Orientation3D.h"
#include "Components/Position3D.h"
#include "Components/Velocity3D.h"
#include "Components/Scale2D.h"
#include "Components/score.h"
#include "Components/Drawable.h"
#include "Components/Collider.h"
#include "Components/Tag.h"

#include "Shaders.h"

using component_list = std::tuple<
	Components::Position3D,
	Components::Orientation3D,

	Components::Position2D,
	Components::Velocity2D,
	Components::Scale2D,

	Components::Camera2D,
	Components::CameraID,
	Components::Input,
	components::score,
	components::collider,

	components::drawable<shaders::sprite_shader>,
	components::drawable<speck::graphics::text>,

	// Tags
	components::brick,
	components::paddle,
	components::ball,
	components::wall,
	components::powerup,
	components::pointer
>;

using filter_list = std::tuple<
	// Movement
	ecs::filter<Components::Position2D, Components::Velocity2D>,

	// Input
	ecs::filter<Components::Input, Components::Position2D>,

	// Rendering
	ecs::filter<components::drawable<shaders::sprite_shader>, Components::Position2D, Components::Scale2D>,
	ecs::filter<components::drawable<speck::graphics::text>, Components::Position2D, Components::Scale2D>,

	// Camera Finding
	ecs::filter<Components::Camera2D>,

	// Camera view matrix updates
	ecs::filter<Components::Camera2D, Components::Position3D, Components::Orientation3D>,

	// Physics
	ecs::filter<components::collider, Components::Position2D, Components::Scale2D>,

	// Score
	ecs::filter<components::score, components::drawable<speck::graphics::text>, 
		Components::Scale2D, Components::Position2D>,

	ecs::filter<components::score>,

	// Tags
	ecs::filter<components::brick>,
	ecs::filter<components::paddle>,
	ecs::filter<components::wall>,
	ecs::filter<components::powerup>,
	ecs::filter<components::ball>,
	ecs::filter<components::pointer>
>;

using ecs_manager = ecs::base_manager<component_list, filter_list>;
using MySystem = ecs::System<ecs_manager>;
