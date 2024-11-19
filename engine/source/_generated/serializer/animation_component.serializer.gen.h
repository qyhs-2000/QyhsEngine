#pragma once
#include "runtime\function\framework\component\animation\animation_component.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const AnimationComponet&instance);
	template<>
	AnimationComponet & Serializer::read(const Json & json_context,AnimationComponet & instance);
}//namespace

