#pragma once
#include <mutex>
#include <optional>
#include <queue>

namespace memory
{
	template<class T>
	class safe_dynamic_queue
	{
	public:
		safe_dynamic_queue() {}

		safe_dynamic_queue(safe_dynamic_queue&& o)
		{
			this->queue = std::move(o.queue);
		}

		void push(T t)
		{
			std::scoped_lock<std::mutex> sl(lock);
			queue.push(t);
		}

		T& peek()
		{
			std::scoped_lock<std::mutex> sl(lock);
			return queue.front();
		}

		T pop()
		{
			std::scoped_lock<std::mutex> sl(lock);
			auto front = std::move(queue.front());
			queue.pop();
			return front;
		}

		size_t size()
		{
			std::scoped_lock<std::mutex> sl(lock);
			return queue.size();
		}

	private:
		std::queue<T> queue;
		std::mutex lock;
	};
}
