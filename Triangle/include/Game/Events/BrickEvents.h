#pragma once
#include <stdint.h>

namespace events
{
	struct brick_hit_event
	{
		uint32_t brick_id, ball_id;
	};
}