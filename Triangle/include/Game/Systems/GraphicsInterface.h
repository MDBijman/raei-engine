#pragma once
#include "Modules/ECS/ECS.h"
#include "Game/GameConfig.h"
#include "Game/Components/SpriteShader.h"
#include "Modules/Graphics/Logic/Renderer.h"

namespace Systems
{
	class GraphicsInterface : public MySystem
	{
	public:
		explicit GraphicsInterface(Graphics::Renderer* graphics) : graphics(graphics) {}

		void update(MyECSManager& ecs, double dt) override
		{
			auto frame = graphics->getFrame();

			auto entities = ecs.filterEntities<Filter<Components::CommandBuffers>>();
			for (auto entity : entities)
			{
				auto& buffers = ecs.getComponent<Components::CommandBuffers>(entity);
				
				frame.addCommandBuffer(buffers.commandBuffers->at(graphics->getCurrentBuffer()));
			}
			
			graphics->submitFrame(frame);
		}

	private:
		Graphics::Renderer* graphics;
	};
}
