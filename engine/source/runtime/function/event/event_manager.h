#pragma once
#include <mutex>
namespace QYHS
{

	inline constexpr int EVENT_THREAD_SAFE_POINT = -1;
	class EventManager
	{
	public:
		std::mutex locker;
		void fireEvent(int id);
	private:

	};



}