#pragma once
#include "runtime\function\framework\component\motor\motor_component.h"
#include "_generated\serializer\component.serializer.gen.h"

namespace QYHS
{
	//template<>
	//Json Serializer::write(const MotorComponentRes&instance);
	template<>
	MotorComponentRes & Serializer::read(const Json & json_context,MotorComponentRes & instance);
	//template<>
	//Json Serializer::write(const MotorComponent&instance);
	template<>
	MotorComponent & Serializer::read(const Json & json_context,MotorComponent & instance);
}//namespace

