#pragma once
#include <memory>
#include "Modules/Graphics/Graphics.h"
#include "Modules/Graphics/Drawables/Text.h"
#include "../Systems/Systems.h"
#include "../ECSConfig.h"
#include "../EventConfig.h"

namespace game
{
	void load(std::shared_ptr<ecs_manager> ecs, std::shared_ptr<event_manager>& events,
		graphics::Camera& graphics_camera, graphics::Renderer& graphics)
	{
		auto title = ecs->createEntity();
		{
			ecs->addComponent(title, Components::Position2D(0.0, 0.0));
			ecs->addComponent(title, Components::Scale2D(.3, .3));
			ecs->addComponent(title, components::drawable<speck::graphics::text>(
				graphics.factory.create_text("ASD", graphics_camera)));
		}

		{
			auto render_group = ecs->get_system_manager().create_group();
			ecs->get_system_manager().add_to_group(render_group, std::make_unique<Systems::GraphicsInterface>(
				Systems::GraphicsInterface(&graphics)));
			ecs->get_system_manager().add_to_group(render_group, std::make_unique<Systems::Exit>(
				Systems::Exit()));
		}
	}
}