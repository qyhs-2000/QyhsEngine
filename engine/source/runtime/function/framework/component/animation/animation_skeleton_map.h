#pragma once
#include "core/meta/reflection/reflection.h"
namespace QYHS
{
	REFLECTION_TYPE(AnimationSkeletonMap)
		CLASS(AnimationSkeletonMap, Fields)
	{
		REFLECTION_BODY(AnimationSkeletonMap)
	public:
		std::vector<int> convert;
	};
}