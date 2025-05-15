#pragma once
#include "runtime\function\framework\component\animation\animation_component.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const AnimationComponent&instance);
	template<>
	AnimationComponent & Serializer::read(const Json & json_context,AnimationComponent & instance);
}//namespace

