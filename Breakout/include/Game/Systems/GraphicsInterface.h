#pragma once
#include <vector>
#include "Modules/ECS/ECSManager.h"
#include "Modules/Graphics/Core/Renderer.h"
#include "Modules/Graphics/Drawables/Text.h"
#include "Game/Components/CommandBuffers.h"
#include "Game/ECSConfig.h"

namespace Systems
{
	class GraphicsInterface : public MySystem
	{
	public:
		explicit GraphicsInterface(graphics::Renderer* graphics) : graphics(graphics) {}

		void update(ecs_manager& ecs) override
		{
			auto frame = graphics->getFrame();

			auto result = ecs.filterEntities<ecs::filter<components::drawable<sprite_shader>>>();
			for (auto entity : result.entities)
			{
				auto& drawable = ecs.getComponent<components::drawable<sprite_shader>>(entity);
				frame.add_drawable<sprite_shader>(drawable);
			}

			auto scores = ecs.filterEntities<ecs::filter<components::score>>();
			for (auto e : scores.entities)
			{
				auto& score = ecs.getComponent<components::score>(e);
				frame.add_drawable(score);
			}

			graphics->submitFrame(&frame);
		}

	private:
		graphics::Renderer* graphics;
	};
}
