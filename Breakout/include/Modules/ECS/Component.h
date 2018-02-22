#pragma once
#include <stdint.h>

namespace ecs
{
	class Component
	{
	public:

	private:
		template<class A, class B>
		friend class base_manager;

		uint32_t parent;
	};
}
