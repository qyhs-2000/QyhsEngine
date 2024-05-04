#pragma once
#include "runtime\resource\type\data\material.h"

namespace QYHS
{
	//template<>
	//Json Serializer::write(const MaterialRes&instance);
	template<>
	MaterialRes & Serializer::read(const Json & json_context,MaterialRes & instance);
}//namespace

