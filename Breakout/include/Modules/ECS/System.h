#pragma once

namespace ecs
{
	template<class ECS>
	class System
	{
	public:
		virtual ~System()
		{
		}

		virtual void update(ECS& ecs) = 0;
	};
}
