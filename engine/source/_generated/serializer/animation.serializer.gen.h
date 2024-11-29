#pragma once
#include "runtime\function\framework\component\animation\animation.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const BlendState&instance);
	template<>
	BlendState & Serializer::read(const Json & json_context,BlendState & instance);
	template<>
	Json Serializer::write(const AnimationComponentRes&instance);
	template<>
	AnimationComponentRes & Serializer::read(const Json & json_context,AnimationComponentRes & instance);
}//namespace

