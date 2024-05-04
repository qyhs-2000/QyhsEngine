#include "game_object_desc.h"

namespace QYHS
{
	int QYHS::GameObjectDesc::getObjectPartSize()
	{
		return m_object_parts.size();
	}

	std::vector<GameObjectPartDesc>& GameObjectDesc::getGameObjectParts()
	{
		return m_object_parts;
	}

	
}

