#include "render_camera.h"


namespace QYHS
{
	RenderCamera::RenderCamera()
	{
		initialize();
	}

	void RenderCamera::zoom(float offset)
	{
		m_fovx = Math::clamp(m_fovx - offset, MIN_FOV, MAX_FOV);
	}


	void RenderCamera::initialize()
	{
		last_camera_x_pos = 2500 / 2;
		last_camera_y_pos = 1600 / 2;
	}

	void RenderCamera::rotate(Vector2 delta)
	{
		delta = Vector2(Radian(Degree(delta.x)).valueRadians(), Radian(Degree(delta.y)).valueRadians());
		float dot = m_up_axis.dotProduct(forward());
		if ((dot < -0.99f && delta.x>0.0f) || (dot > 0.99f && delta.x < 0))
		{
			delta.x = 0.f;
		}

		Quaternion pitch, yaw;
		pitch.fromAngleAxis(Radian(delta.x), X);
		yaw.fromAngleAxis(Radian(delta.y), Z);
		m_rotation = pitch * m_rotation * yaw;
		m_invRotation = m_rotation.conjugate();
	}

	void RenderCamera::changeType(RenderCameraType camera_type)
	{
		m_current_camera_type = camera_type;
	}

	Matrix4x4 QYHS::RenderCamera::getViewMatrix() 
	{
		std::lock_guard< std::mutex> lock_guard(m_view_matrix_mutex);
		Matrix4x4 view_matrix;
		switch (m_current_camera_type)
		{
		case RenderCameraType::Editor:
			view_matrix = Math::makeLookAtMatrix(m_position, getCameraPos() + forward(), up());
			break;
		case RenderCameraType::Motor:
			//view_matrix = Math::makeLookAtMatrix(m_position, getCameraPos() + forward(), up());
			view_matrix = m_view_matrix;
			break;
		default:
			break;
		}
		return view_matrix;
	}

	Matrix4x4 RenderCamera::getProjMatrix() 
	{
		Matrix4x4 proj_matrix = Math::makePerspectiveMatrix(Radian(Degree(m_fovy)), m_aspect, m_znear, m_zfar);
		proj_matrix[1][1] *= -1;
		return proj_matrix;
	}

	void RenderCamera::setAspect(float aspect)
	{
		m_aspect = aspect;
		m_fovy = Radian(Math::atan(Math::tan(Radian(Degree(m_fovx) * 0.5f)) / m_aspect) * 2.0f).valueDegrees();
	}

	void RenderCamera::lookAt(Vector3 position, Vector3 target, Vector3 up)
	{
		m_position = position;
		Vector3 forward = (target - position).normalisedCopy();
		Vector3 right = forward.crossProduct(up).normalisedCopy();
		Vector3 upNormalized = right.crossProduct(forward).normalisedCopy();

		Quaternion up_rotation = (m_rotation * upNormalized).getRotationTo(Z);

		m_rotation = Quaternion(up_rotation) * m_rotation;
		m_invRotation = m_rotation.conjugate();
	}

	void RenderCamera::setMainViewMatrix(Matrix4x4 view_matrix)
	{
		std::lock_guard<std::mutex> lock_guard(m_view_matrix_mutex);
		m_view_matrix = view_matrix;
		Vector3 s  = Vector3(view_matrix[0][0], view_matrix[0][1], view_matrix[0][2]);
		Vector3 u  = Vector3(view_matrix[1][0], view_matrix[1][1], view_matrix[1][2]);
		Vector3 f  = Vector3(-view_matrix[2][0], -view_matrix[2][1], -view_matrix[2][2]);
		m_position = s * (-view_matrix[0][3]) + u * (-view_matrix[1][3]) + f * view_matrix[2][3];
	}

	void RenderCamera::setCurrentCameraType(RenderCameraType camera_type)
	{
		std::lock_guard<std::mutex> lock_guard(m_view_matrix_mutex);
		m_current_camera_type = camera_type;
	}


}

