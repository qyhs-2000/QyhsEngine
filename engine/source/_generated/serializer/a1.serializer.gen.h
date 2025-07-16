#pragma once
#include "runtime\function\render\a1.h"
#include "_generated\serializer\component.serializer.gen.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const AnimComponentTest111&instance);
	template<>
	AnimComponentTest111 & Serializer::read(const Json & json_context,AnimComponentTest111 & instance);
}//namespace

