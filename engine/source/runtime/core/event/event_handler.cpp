#include "event_handler.h"

namespace qyhs::eventhandler
{
	std::shared_ptr<EventManager> manager = std::make_shared<EventManager>();
	void subscribe_once(int id, std::function<void(uint64_t)> callback)
	{
		manager->locker.lock();
		manager->subscribers_onces[id].push_back(callback);
		manager->locker.unlock();
	}

	void subscribe(int id, std::function<void(uint64_t)> callback)
	{
		manager->locker.lock();
		manager->subscribers[id].push_back(callback);
		manager->locker.unlock();
	}
	void fireEvent(int id, uint64_t userdata)
	{
		{
			//subscriber onces
			manager->locker.lock();
			auto iter = manager->subscribers_onces.find(id);
			bool found = iter != manager->subscribers_onces.end();
			manager->locker.unlock();

			if (found)
			{
				auto& callbacks = iter->second;
				for (auto& callback : iter->second)
				{
					callback(userdata);
				}
				callbacks.clear();
			}
		}

		{
			//subscribers
			manager->locker.lock();
			auto iter = manager->subscribers.find(id);
			bool found = iter != manager->subscribers.end();
			manager->locker.unlock();

			if (found)
			{
				auto& callbacks = iter->second;
				for (auto& callback : iter->second)
				{
					callback(userdata);
				}
			}
		}

	}
}