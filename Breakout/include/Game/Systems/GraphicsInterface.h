#pragma once
#include <vector>
#include "Modules/ECS/ECSManager.h"
#include "Modules/Graphics/Core/Renderer.h"
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
				auto& shader = ecs.getComponent<components::drawable<sprite_shader>>(entity);

				frame.addCommandBuffer(&shader.buffers().at(graphics->getCurrentBuffer()));
			}
			graphics->submitFrame(&frame);
		}

	private:
		graphics::Renderer* graphics;
	};
}
