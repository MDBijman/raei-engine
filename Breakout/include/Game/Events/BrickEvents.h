#pragma once
#include <stdint.h>

namespace events
{
	struct collision
	{
		uint32_t a, b;
		collision(uint32_t a, uint32_t b) : a(a), b(b) {}
	};
}