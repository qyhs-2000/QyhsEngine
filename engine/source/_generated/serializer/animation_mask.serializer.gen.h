#pragma once
#include "runtime\function\framework\component\animation\animation_mask.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const BoneBlendMask&instance);
	template<>
	BoneBlendMask & Serializer::read(const Json & json_context,BoneBlendMask & instance);
}//namespace

