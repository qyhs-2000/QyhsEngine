#pragma once
#include "runtime\resource\common\object.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const ObjectDefinitionResource&instance);
	template<>
	ObjectDefinitionResource & Serializer::read(const Json & json_context,ObjectDefinitionResource & instance);
	template<>
	Json Serializer::write(const ObjectInstanceResource&instance);
	template<>
	ObjectInstanceResource & Serializer::read(const Json & json_context,ObjectInstanceResource & instance);
}//namespace

