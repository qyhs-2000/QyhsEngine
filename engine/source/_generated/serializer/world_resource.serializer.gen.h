#pragma once
#include "runtime\resource\type\world_resource.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const WorldResource&instance);
	template<>
	WorldResource & Serializer::read(const Json & json_context,WorldResource & instance);
}//namespace

