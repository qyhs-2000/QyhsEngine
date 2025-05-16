#pragma once
#include "runtime\function\framework\component\camera\camera_component.h"
#include "_generated\serializer\component.serializer.gen.h"

namespace qyhs
{
	template<>
	Json Serializer::write(const CameraParamter&instance);
	template<>
	CameraParamter & Serializer::read(const Json & json_context,CameraParamter & instance);
	template<>
	Json Serializer::write(const ThirdPersonCameraParameter&instance);
	template<>
	ThirdPersonCameraParameter & Serializer::read(const Json & json_context,ThirdPersonCameraParameter & instance);
	template<>
	Json Serializer::write(const CameraComponentResource&instance);
	template<>
	CameraComponentResource & Serializer::read(const Json & json_context,CameraComponentResource & instance);
	template<>
	Json Serializer::write(const CameraComponent&instance);
	template<>
	CameraComponent & Serializer::read(const Json & json_context,CameraComponent & instance);
}//namespace

