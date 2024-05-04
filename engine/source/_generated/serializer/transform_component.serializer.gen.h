#pragma once
#include "runtime\function\framework\component\transform\transform_component.h"
#include "_generated\serializer\component.serializer.gen.h"

namespace QYHS
{
	//template<>
	//Json Serializer::write(const TransformComponent&instance);
	template<>
	TransformComponent & Serializer::read(const Json & json_context,TransformComponent & instance);
}//namespace

