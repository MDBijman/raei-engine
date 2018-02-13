#pragma once
#include "Modules/ECS/Component.h"

namespace Components
{
	class CameraID : public ecs::Component
	{
	public:
		explicit CameraID(uint32_t id) : id(id) {}

	private:
		uint32_t id;
	};
}
