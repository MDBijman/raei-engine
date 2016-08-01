#pragma once
#include "Entity.h"

class Component
{
public:

private:
	template<class A, class B>
	friend class ECSManager;

	uint32_t parent;
};
