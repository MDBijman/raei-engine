#pragma once

template<class CT, class FT>
class System
{
public:
	virtual ~System()
	{
	}

	virtual void update(ECSManager<CT, FT>& ecs, double dt) = 0;
};
