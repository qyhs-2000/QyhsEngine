#pragma once
#include "runtime\core\math\quat.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const TmpTest&instance);
	template<>
	TmpTest & Serializer::read(const Json & json_context,TmpTest & instance);
	template<>
	Json Serializer::write(const Quaternion&instance);
	template<>
	Quaternion & Serializer::read(const Json & json_context,Quaternion & instance);
}//namespace

