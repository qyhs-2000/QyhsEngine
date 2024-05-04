#include "game_object_id_allocator.h"

namespace QYHS
{
	GameObjectID GameObjectIdAllocator::allocateId()
	{
		for (size_t index = 0; index < m_allocated_id.size(); ++index)
		{
			auto id = index + 1;
			if (m_allocated_id.find(id) == m_allocated_id.end())
			{
				return id;
			}
		}
		return 0;
	}
}