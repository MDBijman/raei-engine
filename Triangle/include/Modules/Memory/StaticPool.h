#pragma once
#include <assert.h>
#include "Pool.h"



namespace Assets
{
	template<int SIZE, class T>
	class StaticPool : Pool<T>
	{
	public:
		ObjectID create() override
		{
			for(uint32_t i = 0; i < SIZE; i++)
			{
				if(!used[i])
				{
					used[i] = true;
					data[i] = T();
					return i;
				}
			}

			return -1;
		}

		T& get(ObjectID id) override
		{
			assert(id < SIZE && id >= 0);
			assert(used[id] == true);

			return data[id];
		}

		void remove(ObjectID id) override
		{
			assert(id < SIZE && id >= 0);

			used[id] = false;
		}

		void clear() override
		{
			for(int i = 0; i < SIZE; i++)
			{
				used[i] = false;
			}
		}

	private:
		T		data[SIZE] = {};
		bool	used[SIZE] = { false };
	};
}