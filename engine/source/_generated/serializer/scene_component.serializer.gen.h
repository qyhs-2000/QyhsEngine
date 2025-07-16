#pragma once
#include "runtime\function\render\scene_component.h"
#include "_generated\serializer\component.serializer.gen.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const AnimComponentTest&instance);
	template<>
	AnimComponentTest & Serializer::read(const Json & json_context,AnimComponentTest & instance);
}//namespace

