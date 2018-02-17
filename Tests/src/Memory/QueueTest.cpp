#include <catch/catch.hpp>
#include <iostream>
#include "Modules/Memory/Queue.h"

SCENARIO("Ring buffers can dequeue and enqueue items", "[RingBuffer]")
{
	GIVEN("An empty ring buffer")
	{
		Memory::RingBuffer<5, int> queue;

		WHEN("An item is enqueued")
		{
			bool enqueued = queue.enqueue(3);
			REQUIRE(enqueued);

			THEN("The size should increase")
			{
				REQUIRE(queue.size() == 1);
			}

			THEN("The same item can be dequeued")
			{
				int value = queue.dequeue().value();
				REQUIRE(value == 3);
				
				THEN("The size should be zero again")
				{
					REQUIRE(queue.size() == 0);
				}
			}

			THEN("The same item can be peeked")
			{
				int value = queue.peek().value();
				REQUIRE(value == 3);

				THEN("The size should be the same")
				{
					REQUIRE(queue.size() == 1);
				}
			}
		}

		THEN("Calling dequeue should return an empty optional")
		{
			REQUIRE(!queue.dequeue().has_value());
		}

		THEN("Calling peek should return an empty optional")
		{
			REQUIRE(!queue.peek().has_value());
		}
	}


	GIVEN("A full ring buffer")
	{
		Memory::RingBuffer<3, int> queue;
		queue.enqueue(1);
		queue.enqueue(1);
		queue.enqueue(1);

		THEN("The size should be the total size")
		{
			REQUIRE(queue.size() == 3);
		}

		THEN("Calling enqueue should return false")
		{
			REQUIRE(!queue.enqueue(1));
		}
	}
}