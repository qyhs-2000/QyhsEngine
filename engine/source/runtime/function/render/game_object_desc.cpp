#include "game_object_desc.h"

namespace qyhs
{
	int qyhs::GameObjectDesc::getObjectPartSize()
	{
		return m_object_parts.size();
	}

	std::vector<GameObjectPartDesc>& GameObjectDesc::getGameObjectParts()
	{
		return m_object_parts;
	}

	
}

