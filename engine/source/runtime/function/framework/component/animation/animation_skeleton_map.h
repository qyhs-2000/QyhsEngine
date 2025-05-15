#pragma once
#include "core/meta/reflection/reflection.h"
namespace qyhs
{
	REFLECTION_TYPE(AnimationSkeletonMap)
		CLASS(AnimationSkeletonMap, Fields)
	{
		REFLECTION_BODY(AnimationSkeletonMap)
	public:
		std::vector<int> convert;
	};
}