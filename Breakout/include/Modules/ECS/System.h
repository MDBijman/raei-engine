#pragma once

namespace ecs
{
	template<class CT, class FT>
	class System
	{
	public:
		virtual ~System()
		{
		}

		virtual void update(base_manager<CT, FT>& ecs) = 0;
	};
}
