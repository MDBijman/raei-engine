#pragma once
#include "Components\Components.h"
#include "Modules\ECS\ECS.h"
#include "Modules\AssetManager\AssetManager.h"
#include "Modules\Graphics\Data\Textures\Texture.h"

using MyComponentList = ComponentList<
	// Transform components
	Components::Position2D,
	Components::Velocity2D,

	// Graphics components
	Components::Camera2D,
	Components::Texture,
	Components::Mesh,

	// Other components
	Components::Input
>;

using MyFilterList = FilterList<
	// Movement
	Filter<Components::Position2D>,

	// Input
	Filter<Components::Input, Components::Position2D, Components::Velocity2D>,

	// GraphicsInterface
	Filter<Components::Texture, Components::Mesh>
>;

using MyECSManager = ECSManager<MyComponentList, MyFilterList>;
using MySystem = System<MyComponentList, MyFilterList>;

using MyAssetManager = AssetManager::AssetManager<
	Graphics::Data::Texture
>;
