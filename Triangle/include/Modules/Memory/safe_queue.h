#pragma once
#include <mutex>
#include <optional>

namespace memory
{
	template<int SIZE, class T>
	class safe_queue
	{
	public:
		static_assert(SIZE > 0, "Queue size must be greater than 0");

		safe_queue() : head(0), tail(0), count(0), lock() {}

		safe_queue(safe_queue&& o) : lock(std::mutex()), head(o.head), tail(o.tail), count(o.count), 
			data(std::move(o.data)) {}

		bool enqueue(T t)
		{
			std::lock_guard<std::mutex> lg(lock);
			if(count == SIZE)
				return false;

			data[tail] = std::move(t);
			tail = (tail + 1) % SIZE;
			count++;
			return true;
		}

		std::optional<T> dequeue()
		{
			std::lock_guard<std::mutex> lg(lock);
			if(count == 0)
				return std::nullopt;

			tail--;
			count--;
			return std::make_optional(data[tail]);
		}

		size_t size()
		{
			std::lock_guard<std::mutex> lg(lock);
			return count;
		}

	private:
		std::mutex lock;

		std::array<T, SIZE> data;
		int head, tail;
		int count;
	};
}
