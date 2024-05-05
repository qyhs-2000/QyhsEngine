#pragma once
#include "runtime\core\math\vector2.h"

namespace QYHS
{
	//template<>
	//Json Serializer::write(const Vector2&instance);
	template<>
	Vector2 & Serializer::read(const Json & json_context,Vector2 & instance);
}//namespace

