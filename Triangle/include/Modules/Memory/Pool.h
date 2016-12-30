#pragma once

namespace Assets
{
	template<class T>
	class Pool
	{
	public:
		using ObjectID = uint32_t;

		ObjectID create() = 0;
		T& get(ObjectID id) = 0;
		void remove(ObjectID id) = 0;
		void clear() = 0;
	};
}