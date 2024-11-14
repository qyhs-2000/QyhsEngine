#pragma once
#include "../component.h"
//#include "core/meta/reflection/reflection.h"
#include "core/math/quat.h"
#include "core/math/vector3.h"
namespace QYHS
{
	enum class CameraMode
	{
		ThirdPersonMode = 0,
		FirstPersonMode,
		FreeMode,
		Invalid
	};

	REFLECTION_TYPE(CameraParamter)
		CLASS(CameraParamter, Fields)
	{
		REFLECTION_BODY(CameraParamter)
	public:
		float m_fov {50.f};
		virtual ~CameraParamter() {}
	};

	REFLECTION_TYPE(ThirdPersonCameraParameter)
		CLASS(ThirdPersonCameraParameter:public CameraParamter, Fields)
	{
		REFLECTION_BODY(ThirdPersonCameraParameter)
	public:
		META(Enable)
		float m_horizontal_offset;
		META(Enable)
		float m_vertical_offset;
		Quaternion m_cursor_pitch;
		Quaternion m_cursor_yaw;
	};

	REFLECTION_TYPE(CameraComponentResource)
		CLASS(CameraComponentResource, Fields)
	{
		REFLECTION_BODY(CameraComponentResource)
	public:
		Reflection::ReflectionPtr<CameraParamter>  m_parameter;
	private:
	};

	REFLECTION_TYPE(CameraComponent)
		CLASS(CameraComponent:public Component,WhiteListFields)
	{
		REFLECTION_BODY(CameraComponent)
	public:
		virtual void tick(double delta_time) override;
		virtual void postLoadResource(std::weak_ptr<GameObject> parent_object) override;
		void tickThirdPersonCamera(double delta_time);
	private:
		META(Enable)
		CameraComponentResource m_camera_res;
		CameraMode m_camera_mode;
		Vector3 m_position;
		
		Vector3 m_forward {Vector3::NEGATIVE_UNIT_Y};
		Vector3 m_up {Vector3::UNIT_Z};
		Vector3 m_left {Vector3::UNIT_X};
	};
}