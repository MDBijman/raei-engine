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
#include "Components/score.h"
#include "Components/Drawable.h"

using sprite_attributes = graphics::data::attributes<
	graphics::data::Vec2<0, 0>,
	graphics::data::Vec2<1, sizeof(float) * 2>
>;
using sprite_uniforms = graphics::data::Uniforms<
	graphics::data::buffer_template<glm::mat4, 0, vk::ShaderStageFlagBits::eVertex>,
	graphics::data::texture_template<1, vk::ShaderStageFlagBits::eFragment>
>;
using sprite_shader = graphics::shader<sprite_attributes, sprite_uniforms>;

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
	Components::sprite_shader,
	Components::Input,
	Components::Scale2D,
	components::score,
	components::drawable<graphics::shader<sprite_attributes, sprite_uniforms>>
>;

using filter_list = std::tuple <
	// Movement
	ecs::filter<Components::Position2D, Components::Velocity2D>,

	// Input
	ecs::filter<Components::Input, Components::Position2D>,

	// Rendering
	ecs::filter<components::drawable<sprite_shader>>,
	// Sprite uniform updates
	ecs::filter<components::drawable<sprite_shader>, Components::Position2D, Components::Scale2D>,

	// Camera Finding
	ecs::filter<Components::Camera2D>,

	// Camera view matrix updates
	ecs::filter<Components::Camera2D, Components::Position3D, Components::Orientation3D>,

	// Physics
	ecs::filter<Components::Position2D, Components::Scale2D>,

	// Score
	ecs::filter<components::score, components::drawable<sprite_shader>, Components::Scale2D>
> ;

using ecs_manager = ecs::base_manager<component_list, filter_list>;
using MySystem = ecs::System<component_list, filter_list>;
