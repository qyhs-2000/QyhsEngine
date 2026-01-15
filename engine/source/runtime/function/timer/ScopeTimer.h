#pragma once
#include <string>
#include <chrono>
#include <iostream>
namespace qyhs
{
	class ScopeTimer
	{
	public:
		ScopeTimer(const std::string& name):m_name(name),m_start_time(std::chrono::high_resolution_clock::now())
		{

		}
		~ScopeTimer()
		{
			auto end_time = std::chrono::high_resolution_clock::now();
			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_start_time).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count();

			double ms = (end - start) * 0.001; // ×ª»»ÎªºÁÃë
			std::cout << "[Timer] " << m_name << " took: " << ms << "ms" << std::endl;
		}
	private:
		std::string m_name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
	};

}