#pragma once
#include "runtime\core\math\matrix4.h"

namespace QYHS
{
	//template<>
	//Json Serializer::write(const Matrix4x4_&instance);
	template<>
	Matrix4x4_ & Serializer::read(const Json & json_context,Matrix4x4_ & instance);
}//namespace

