#pragma once
#include "Game/ECSConfig.h"

namespace graphics
{
	class Renderer;
}

namespace Systems
{
	class GraphicsInterface : public MySystem
	{
	public:
		explicit GraphicsInterface(graphics::Renderer* graphics) : graphics(graphics) {}

		void update(ecs_manager& ecs) override;

	private:
		graphics::Renderer* graphics;
	};
}
