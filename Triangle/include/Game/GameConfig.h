#pragma once
#include "Components\Components.h"
#include "Modules\ECS\ECS.h"
#include "Modules\AssetManager\AssetManager.h"
#include "Modules\Graphics\Data\Textures\Texture.h"

using MyComponentList = ComponentList<
	// Transform components
	Components::Position3D, Components::Velocity3D, Components::Orientation3D,
	Components::Camera,
	Components::Input
>;

using MyFilterList = FilterList<
	Filter<Components::Position3D, Components::Velocity3D>, // Movement System
	Filter<Components::Input, Components::Velocity3D, Components::Orientation3D> // Input System
>;

using MyECSManager = ECSManager<MyComponentList, MyFilterList>;
using MySystem = System<MyComponentList, MyFilterList>;

using MyAssetManager = AssetManager::AssetManager<
	Graphics::Data::Texture
>;
