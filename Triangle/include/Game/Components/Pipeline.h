#pragma once
#include "Modules/ECS/Component.h"
#include "Modules/Importers/PipelineImporter.h"
#include "Modules/Graphics/Graphics.h"

namespace Components
{
	class Pipeline : public ecs::Component
	{
	public:
		Pipeline(graphics::Pipeline p) : pipeline(p)
		{
		}

		graphics::Pipeline pipeline;
	};
}

