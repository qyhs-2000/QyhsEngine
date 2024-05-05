#include "game_object_id_allocator.h"
#include "core/base/macro.h"
namespace QYHS
{
	std::atomic<GameObjectID> GameObjectIdAllocator::m_next_id{ 0 };
	GameObjectID GameObjectIdAllocator::allocateId()
	{
        std::atomic<GameObjectID> new_object_ret = m_next_id.load();
        m_next_id++;
        if (m_next_id >= k_invalid_gobject_id)
        {
            LOG_ERROR("gobject id overflow");
        }

        return new_object_ret;
	}
}