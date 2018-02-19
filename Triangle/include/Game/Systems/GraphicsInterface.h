#pragma once
#include "Modules/ECS/ECSManager.h"
#include "Game/Components/CommandBuffers.h"
#include "Modules/Graphics/Core/Renderer.h"
#include <vector>

namespace Systems
{
	class GraphicsInterface : public MySystem
	{
	public:
		explicit GraphicsInterface(graphics::Renderer* graphics) : graphics(graphics) {}

		void update(ecs_manager& ecs) override
		{
			auto frame = graphics->getFrame();
			auto&[lock, entities] = ecs.filterEntities<ecs::filter<Components::CommandBuffers>>();
			for (auto entity : entities)
			{
				auto& buffers = ecs.getComponent<Components::CommandBuffers>(entity);

				frame.addCommandBuffer(&buffers.commandBuffers->at(graphics->getCurrentBuffer()));
			}
			graphics->submitFrame(&frame);
		}

	private:
		graphics::Renderer* graphics;
	};
}
