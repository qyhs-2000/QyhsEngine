#pragma once

#include "core/math/vector3.h"
#include "resource/common/object.h"
#include <string>
#include<vector>
namespace qyhs
{
	REFLECTION_TYPE(WorldResource)
		CLASS(WorldResource, Fields)
	{
	public:
		
		std::string m_default_level_url;
	private:
	};
}