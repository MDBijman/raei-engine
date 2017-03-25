#pragma once
#include "Modules/ECS/Component.h"

namespace Components
{
	class CameraID : public ECS::Component
	{
	public:
		explicit CameraID(uint32_t id) : id(id) {}

	private:
		uint32_t id;
	};
}
