#pragma once

#include "Components/Components.h"
#include "Modules/ECS/ECS.h"

using MyComponentList = ComponentList<
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

using MyFilterList = FilterList<
	// Movement
	Filter<Components::Position2D, Components::Velocity2D>,

	// Input
	Filter<Components::Input, Components::Position2D>,

	// Rendering
	Filter<Components::CommandBuffers>,

	// Sprite uniform updates
	Filter<Components::SpriteShader, Components::Position2D, Components::Scale2D>,

	// Camera Finding
	Filter<Components::Camera2D>,

	// Camera view matrix updates
	Filter<Components::Camera2D, Components::Position3D, Components::Orientation3D>
>;

using MyECSManager = ECSManager<MyComponentList, MyFilterList>;
using MySystem = System<MyComponentList, MyFilterList>;
