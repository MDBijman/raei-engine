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

using MyComponentList = ECS::ComponentList<
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

using MyFilterList = ECS::FilterList<
	// Movement
	ECS::Filter<Components::Position2D, Components::Velocity2D>,

	// Input
	ECS::Filter<Components::Input, Components::Position2D>,

	// Rendering
	ECS::Filter<Components::CommandBuffers>,

	// Sprite uniform updates
	ECS::Filter<Components::SpriteShader, Components::Position2D, Components::Scale2D>,

	// Camera Finding
	ECS::Filter<Components::Camera2D>,

	// Camera view matrix updates
	ECS::Filter<Components::Camera2D, Components::Position3D, Components::Orientation3D>
>;

using MyECSManager = ECS::ECSManager<MyComponentList, MyFilterList>;
using MySystem = ECS::System<MyComponentList, MyFilterList>;
