#pragma once
#include <chrono>
namespace QYHS
{
	class Timer
	{
	public:
		inline double record_elapsed_time()
		{
			std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - pre_time_slip);
			pre_time_slip = now;
			return time_span.count();
		}
	private:
		std::chrono::high_resolution_clock::time_point pre_time_slip = std::chrono::high_resolution_clock::now();
	};
}
