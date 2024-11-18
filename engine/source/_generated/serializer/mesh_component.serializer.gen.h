#pragma once
#include "runtime\function\framework\component\mesh\mesh_component.h"
#include "_generated\serializer\component.serializer.gen.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const MeshComponent&instance);
	template<>
	MeshComponent & Serializer::read(const Json & json_context,MeshComponent & instance);
}//namespace

