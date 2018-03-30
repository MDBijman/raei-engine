#include "stdafx.h"
#include "Game/Worlds/ScoreList.h"
#include "Game/Systems/CameraSystem.h"
#include "Game/Systems/Exit.h"
#include "Game/Systems/MenuSystem.h"
#include "Game/Systems/GraphicsInterface.h"
#include "Game/Systems/ScoreListSystem.h"
#include "Game/Systems/ScoreSystem.h"

namespace game
{
	world load_scorelist(graphics::Camera & camera, graphics::Renderer & graphics, game::scores& scores)
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
			auto highscores = scores.get_scores();

			double y = -0.95;
			for (decltype(auto) highscore : highscores)
			{
				auto score = ecs.createEntity();

				std::string score_text;
				score_text.resize(3);
				score_text.at(2) = '0' + (highscore % 10);
				score_text.at(1) = '0' + ((highscore / 10) % 10);
				score_text.at(0) = '0' + ((highscore / 100) % 10);

				ecs.addComponent(score, components::score(highscore));
				ecs.addComponent(score, components::drawable<speck::graphics::text>(
					graphics.factory.create_text(score_text, camera)));
				ecs.addComponent(score, Components::Position2D(-0.1, y));
				y += 0.2;
				ecs.addComponent(score, Components::Scale2D(.1, .1));
			}
		}

		systems::score_system(events.new_subscriber<events::collision>()).update(ecs);

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