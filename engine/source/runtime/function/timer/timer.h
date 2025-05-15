#pragma once
#include <chrono>
namespace qyhs
{
	class Timer
	{
	public:
		inline double record_elapsed_time()
		{
			std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - time_stamp);
			time_stamp = now;
			return time_span.count();
		}

		// Elapsed time in seconds between the wi::Timer creation or last recording and "timestamp2"
		inline double elapsed_seconds_since(std::chrono::high_resolution_clock::time_point timestamp2)
		{
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(timestamp2 - time_stamp);
			return time_span.count();
		}
		
		inline double record_elapsed_seconds()
		{
			auto timestamp2 = std::chrono::high_resolution_clock::now();
			auto elapsed = elapsed_seconds_since(timestamp2);
			time_stamp = timestamp2;
			return elapsed;
		}
	private:
		std::chrono::high_resolution_clock::time_point time_stamp = std::chrono::high_resolution_clock::now();
	};
}
