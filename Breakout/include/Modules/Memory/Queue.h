#pragma once
#include <optional>

namespace Memory
{
	template<int SIZE, class T>
	class RingBuffer
	{
	public:
		static_assert(SIZE > 0, "Queue size must be greater than 0");

		RingBuffer()
		{
			head = 0;
			tail = 0;
			count = 0;
		}

		bool enqueue(T t)
		{
			if(count == SIZE)
				return false;

			data[tail] = t;
			tail = (tail + 1) % SIZE;
			count++;
			return true;
		}

		std::optional<T> dequeue()
		{
			if(count == 0)
				return std::optional<T>();

			tail--;
			count--;
			return std::make_optional(data[tail]);
		}

		std::optional<T> peek()
		{
			if(count == 0)
				return std::optional<T>();

			return std::make_optional(data[tail - 1]);
		}

		size_t size()
		{
			return count;
		}

	private:
		T data[SIZE];
		int head, tail;
		int count;
	};
}