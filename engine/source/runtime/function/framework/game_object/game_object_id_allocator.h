#pragma once
#include <unordered_set>
namespace QYHS
{
	using GameObjectID = size_t;
	class GameObjectIdAllocator
	{
	public:
		GameObjectID allocateId();
	private:
		std::unordered_set<GameObjectID>	m_allocated_id;
	};
}