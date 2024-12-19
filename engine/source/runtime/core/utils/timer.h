#pragma once
#include <chrono>
namespace QYHS
{
	class Timer
	{
	public:
		inline double record_past_time();
	private:
		std::chrono::high_resolution_clock::time_point current_time_stamp = std::chrono::high_resolution_clock::now();
	};

	namespace timer
	{
		void QuickSleep(float milliseconds);
	}
}