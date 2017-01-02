#pragma once
#include <optional>

namespace Memory
{
	template<class T>
	class Queue
	{
	public:
		virtual bool enqueue(T t) = 0;
		virtual std::optional<T> dequeue() = 0;
		virtual std::optional<T> peek() = 0;
		virtual size_t size() = 0;
	};

	template<int SIZE, class T>
	class RingBuffer : Queue<T>
	{
	public:
		static_assert(SIZE > 0, "Queue size must be greater than 0");

		RingBuffer()
		{
			head = 0;
			tail = 0;
			count = 0;
		}

		bool enqueue(T t) override
		{
			if(count == SIZE)
				return false;

			data[tail] = t;
			tail = (tail + 1) % SIZE;
			count++;
			return true;
		}

		std::optional<T> dequeue() override
		{
			if(count == 0)
				return std::optional<T>();

			tail--;
			count--;
			return std::make_optional(data[tail]);
		}

		std::optional<T> peek() override
		{
			if(count == 0)
				return std::optional<T>();

			return std::make_optional(data[tail - 1]);
		}

		size_t size() override
		{
			return count;
		}

	private:
		T data[SIZE];
		int head, tail;
		int count;
	};
}