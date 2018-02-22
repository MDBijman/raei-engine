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
			returns the difference between the last start() and end() call as double, in seconds.
		*/
		double dt() const
		{
			return delta / 1000.0;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> time;
		std::chrono::duration<double, std::milli>::rep delta;
	};

	class Timer2
	{
	public:
		Timer2()
		{
			zero();
		}

		/*
		* Zeroes the timer.
		*/
		void zero()
		{
			time = std::chrono::high_resolution_clock::now();
		}

		/*
			returns the difference between the last start() and end() call as double, in seconds.
		*/
		double dt() const
		{
			auto delta = std::chrono::duration<double, std::milli>
				(std::chrono::high_resolution_clock::now() - time).count();
			return delta / 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> time;
	};
}