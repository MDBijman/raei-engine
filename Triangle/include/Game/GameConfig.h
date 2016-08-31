#pragma once
#include "Components\Components.h"
#include "Modules\ECS\ECS.h"
#include "Modules\AssetManager\AssetManager.h"
#include "Modules\Graphics\Data\Textures\Texture.h"

using MyComponentList = ComponentList<
	// Transform components
	Components::Position2D,
	Components::Velocity2D,
	Components::Camera2D,
	Components::Input
>;

using MyFilterList = FilterList<
	Filter<Components::Position2D>, // Movement System
	Filter<Components::Input, Components::Position2D, Components::Velocity2D> // Input System
>;

using MyECSManager = ECSManager<MyComponentList, MyFilterList>;
using MySystem = System<MyComponentList, MyFilterList>;

using MyAssetManager = AssetManager::AssetManager<
	Graphics::Data::Texture
>;
