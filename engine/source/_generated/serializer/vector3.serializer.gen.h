#pragma once
#include "runtime\core\math\vector3.h"

namespace QYHS
{
	template<>
	Json Serializer::write(const Vector3&instance);
	template<>
	Vector3 & Serializer::read(const Json & json_context,Vector3 & instance);
}//namespace

