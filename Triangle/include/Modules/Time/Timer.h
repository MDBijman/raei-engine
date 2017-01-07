#pragma once
#include <chrono>

namespace Time
{
	class Timer
	{
	public:
		/*
			Starts the timer.
		*/
		void start()
		{
			time = std::chrono::high_resolution_clock::now();
		}

		/*
			Ends the timer and sets the dt to time elapsed since start().
		*/
		void stop()
		{
			auto endTime = std::chrono::high_resolution_clock::now();
			delta = std::chrono::duration<double, std::milli>(endTime - time).count();
		}

		/*
			returns the difference between the last start() and end() call as double, in milliseconds.
		*/
		double dt()
		{
			return delta;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> time;
		std::chrono::duration<double, std::milli>::rep delta;
	};
}