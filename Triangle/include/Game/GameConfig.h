#pragma once
#include "Components/Components.h"
#include "Modules/ECS/ECS.h"
#include "Modules/AssetManager/AssetManager.h"
#include "Modules/Graphics/Data/Textures/Texture.h"

using MyComponentList = ComponentList<
	// Transform components
	Components::Position2D,
	Components::Velocity2D,

	// Graphics components
	Components::Camera2D,
	Components::Texture,
	Components::Mesh,
	Components::CameraID,
	Components::Pipeline,
	Components::CommandBuffers,
	Components::MeshShader,

	// Other components
	Components::Input
>;

using GraphicsFilter = Filter<Components::Texture, Components::Mesh, Components::CameraID, Components::CommandBuffers, Components::Pipeline, Components::MeshShader>;

using MyFilterList = FilterList<
	// Movement
	Filter<Components::Position2D>,

	// Input
	Filter<Components::Input, Components::Position2D, Components::Velocity2D>,

	GraphicsFilter,

	// Camera Finding
	Filter<Components::Camera2D>
>;

using MyECSManager = ECSManager<MyComponentList, MyFilterList>;
using MySystem = System<MyComponentList, MyFilterList>;

using MyAssetManager = AssetManager::AssetManager<
	Graphics::Data::Texture
>;
