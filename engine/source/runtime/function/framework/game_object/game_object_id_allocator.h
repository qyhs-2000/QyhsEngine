#pragma once
#include <atomic>
#include <limits>
namespace qyhs
{
	using GameObjectID = size_t;
	constexpr GameObjectID k_invalid_gobject_id = std::numeric_limits<std::size_t>::max();
	class GameObjectIdAllocator
	{
	public:
		GameObjectID allocateId();
	private:
		static std::atomic<GameObjectID> m_next_id;
	};
}