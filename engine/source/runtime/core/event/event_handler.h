#pragma once
#include <functional>
#include <memory>
#include <mutex>
namespace qyhs::eventhandler
{
	inline constexpr int EVENT_THREAD_SAFE_POINT = -1;

	struct EventManager
	{
		std::unordered_map < uint64_t, std::vector<std::function<void(uint64_t)>>> subscribers_onces;
		std::unordered_map < uint64_t, std::vector<std::function<void(uint64_t)>>> subscribers;
		std::mutex locker;
	};

	void subscribe_once(int id, std::function<void(uint64_t)> callback);
	void subscribe(int id, std::function<void(uint64_t)> callback);
	void fireEvent(int id, uint64_t userdata);
}