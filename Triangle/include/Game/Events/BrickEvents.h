#pragma once
#include <stdint.h>

namespace events
{
	struct brick_hit
	{
		brick_hit(uint32_t brick, uint32_t ball) : brick_id(brick), ball_id(ball) {}
		uint32_t brick_id, ball_id;
	};
}