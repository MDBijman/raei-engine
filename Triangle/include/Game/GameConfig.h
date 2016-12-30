#pragma once
#include "Components/Components.h"
#include "Modules/ECS/ECS.h"

using MyComponentList = ComponentList<
	// Transform components
	Components::Position2D,
	Components::Velocity2D,

	// Graphics components
	Components::Camera2D,
	Components::Texture,
	//Components::Mesh,
	Components::CameraID,
	Components::Pipeline,
	Components::CommandBuffers,
	Components::SpriteShader,

	// Other components
	Components::Input
>;

//using GraphicsFilter = Filter<Components::Texture, Components::Mesh, Components::CameraID, Components::CommandBuffers, Components::Pipeline, Components::MeshShader>;
using SpriteFilter = Filter<Components::CommandBuffers, Components::Pipeline, Components::SpriteShader>;

using MyFilterList = FilterList<
	// Movement
	Filter<Components::Position2D, Components::Velocity2D>,

	// Input
	Filter<Components::Input, Components::Position2D, Components::Velocity2D>,

	SpriteFilter,

	Filter<Components::SpriteShader, Components::Position2D>,

	// Camera Finding
	Filter<Components::Camera2D>
>;

using MyECSManager = ECSManager<MyComponentList, MyFilterList>;
using MySystem = System<MyComponentList, MyFilterList>;

