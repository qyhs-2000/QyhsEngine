#pragma once
#include <atomic>
#include <thread>
namespace QYHS
{
	class SpinLock
	{
	public:
		void lock()
		{
			int spin = 0;
			while (!tryLock())
			{
				if (spin < 10)
				{
					_mm_pause();		//stop current thread for a while
				}
				else
				{
					std::this_thread::yield();     //swap to another thread ,abandon current time slip
				}
				++spin;
			}
		}

		bool tryLock()
		{
			return !lck.test_and_set(std::memory_order_acquire);
		}

		void unlock()
		{
			lck.clear(std::memory_order_release);
		}

	private:
		std::atomic_flag lck = ATOMIC_FLAG_INIT;
	};
}