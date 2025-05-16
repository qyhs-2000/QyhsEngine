#pragma once
#include "runtime\resource\type\level_resource.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const LevelResource&instance);
	template<>
	LevelResource & Serializer::read(const Json & json_context,LevelResource & instance);
}//namespace

