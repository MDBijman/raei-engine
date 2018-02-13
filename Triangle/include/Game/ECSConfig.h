#pragma once
#include "Modules/ECS/ECSManager.h"
#include "Components/SpriteShader.h"
#include "Components/Camera2D.h"
#include "Components/CameraID.h"
#include "Components/Input.h"
#include "Components/Position2D.h"
#include "Components/Velocity2D.h"
#include "Components/Orientation3D.h"
#include "Components/Position3D.h"
#include "Components/Velocity3D.h"
#include "Components/Texture.h"
#include "Components/Pipeline.h"
#include "Components/CommandBuffers.h"
#include "Components/Scale2D.h"
#include "Components/Dummy.h"

using component_list = std::tuple<
	Components::Position2D,
	Components::Position3D,
	Components::Orientation3D,
	Components::Velocity2D,
	Components::Camera2D,
	Components::Texture,
	Components::CameraID,
	Components::Pipeline,
	Components::CommandBuffers,
	Components::SpriteShader,
	Components::Input,
	Components::Scale2D
>;

using filter_list = std::tuple<
	// Movement
	ecs::filter<Components::Position2D, Components::Velocity2D>,

	// Input
	ecs::filter<Components::Input, Components::Position2D>,

	// Rendering
	ecs::filter<Components::CommandBuffers>,

	// Sprite uniform updates
	ecs::filter<Components::SpriteShader, Components::Position2D, Components::Scale2D>,

	// Camera Finding
	ecs::filter<Components::Camera2D>,

	// Camera view matrix updates
	ecs::filter<Components::Camera2D, Components::Position3D, Components::Orientation3D>,

	// Physics
	ecs::filter<Components::Position2D, Components::Scale2D>
>;

using ecs_manager = ecs::base_manager<component_list, filter_list>;
using MySystem = ecs::System<component_list, filter_list>;
