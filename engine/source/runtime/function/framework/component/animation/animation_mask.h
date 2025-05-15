#pragma once
#include "core/meta/reflection/reflection.h"
namespace qyhs
{
	REFLECTION_TYPE(BoneBlendMask)
		CLASS(BoneBlendMask, Fields)
	{
		REFLECTION_BODY(BoneBlendMask)
	public:
		std::string skeleton_file_path;
		std::vector<float> enabled;
	};
}