#pragma once
#include "runtime\function\framework\component\hierarchy\hierarchy_component.h"
#include "_generated\serializer\component.serializer.gen.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const HierarchyComponent&instance);
	template<>
	HierarchyComponent & Serializer::read(const Json & json_context,HierarchyComponent & instance);
}//namespace

