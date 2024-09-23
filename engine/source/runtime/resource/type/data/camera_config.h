#pragma once

#include "core/meta/reflection/reflection.h"
#include "core/math/vector3.h"
namespace QYHS
{
	REFLECTION_TYPE(CameraPose)
		CLASS(CameraPose, Fields)
	{
		REFLECTION_BODY(CameraPose)
	public:
		Vector3 m_position;
		Vector3 m_target;
		Vector3 m_up;
	private:
	};

	REFLECTION_TYPE(CameraConfig)
		CLASS(CameraConfig, Fields)
	{
		REFLECTION_BODY(CameraConfig)
	public:
		CameraPose m_pose;
		Vector2 m_aspect;
		float m_z_far;
		float m_z_near;
	private:
	};
}