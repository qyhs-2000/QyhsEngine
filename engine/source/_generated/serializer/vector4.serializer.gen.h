#pragma once
#include "runtime\core\math\vector4.h"

namespace QYHS
{
	//template<>
	//Json Serializer::write(const Vector4&instance);
	template<>
	Vector4 & Serializer::read(const Json & json_context,Vector4 & instance);
}//namespace

