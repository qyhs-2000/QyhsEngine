#pragma once
#include "runtime\function\framework\component\animation\animation_skeleton_map.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const AnimationSkeletonMap&instance);
	template<>
	AnimationSkeletonMap & Serializer::read(const Json & json_context,AnimationSkeletonMap & instance);
}//namespace

