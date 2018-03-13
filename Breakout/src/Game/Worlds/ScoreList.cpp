#include "stdafx.h"
#include "Game/Worlds/ScoreList.h"
#include "Game/Systems/CameraSystem.h"
#include "Game/Systems/Exit.h"
#include "Game/Systems/MenuSystem.h"
#include "Game/Systems/GraphicsInterface.h"
#include "Game/Systems/ScoreListSystem.h"

namespace game
{
	world load_scorelist(graphics::Camera & camera, graphics::Renderer & graphics)
	{
		auto ecs = ecs_manager();
		auto events = event_manager();

		{
			auto camera_entity = ecs.createEntity();
			auto& pos = ecs.addComponent(camera_entity, Components::Position3D(0.0, 0.0, 1.0));
			ecs.addComponent(camera_entity, Components::Orientation3D());
			auto& camera_component = ecs.addComponent(camera_entity, Components::Camera2D(camera));
			camera_component.camera.moveTo(pos.pos, glm::vec3());
		}

		// Scores
		{
			auto score = ecs.createEntity();

			ecs.addComponent(score, components::score(0));
			ecs.addComponent(score, components::drawable<speck::graphics::text>(
				graphics.factory.create_text("000", camera)));
			ecs.addComponent(score, Components::Position2D(0.0, -.99));
			ecs.addComponent(score, Components::Scale2D(.1, .1));
		}

		{
			auto render_group = ecs.get_system_manager().create_group();
			ecs.get_system_manager().add_to_group(render_group, std::make_unique<Systems::CameraSystem>());
			ecs.get_system_manager().add_to_group(render_group, std::make_unique<Systems::GraphicsInterface>(
				Systems::GraphicsInterface(&graphics)));
			ecs.get_system_manager().add_to_group(render_group, std::make_unique<systems::score_list_system>(
				events.new_publisher<events::switch_world>()));
		}

		return world(std::move(ecs), std::move(events));
	}
}