#pragma once
#include "Modules/ECS/ECS.h"

namespace Components
{
	class CameraID : public Component
	{
	public:
		explicit CameraID(uint32_t id) : id(id) {}

	private:
		uint32_t id;
	};
}
