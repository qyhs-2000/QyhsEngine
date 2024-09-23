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

	//TODO: initialize last camera pos

	void RenderCamera::initialize()
	{
		last_camera_x_pos = 2500 / 2;
		last_camera_y_pos = 1600 / 2;
	}

	/*void RenderCamera::updateCursorPos(float x_pos, float y_pos)
	{
		if (first_mouse)
		{
			last_camera_x_pos = x_pos;
			last_camera_y_pos = y_pos;
			first_mouse = false;
		}
		float x_offset = x_pos - last_camera_x_pos;
		float y_offset = y_pos - last_camera_y_pos;
		y_offset *= -1;
		last_camera_x_pos = x_pos;
		last_camera_y_pos = y_pos;

		const float sensitivity = 0.2f;
		x_offset *= sensitivity;
		y_offset *= sensitivity;

		yaw += x_offset;
		pitch = (pitch + y_offset) > 89.f ? 89.f : ((pitch + y_offset) < -89.f ? -89.f : (pitch + y_offset));

		glm::vec3 direction;
		direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		direction.y = sin(glm::radians(pitch));
		direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		camera_front = glm::normalize(direction);
	}*/

	void RenderCamera::updateCameraPosition()
	{
		m_position += camera_pos_offset;
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

	Matrix4x4 QYHS::RenderCamera::getViewMatrix() const
	{
		return Math::makeLookAtMatrix(m_position, getCameraPos() + forward(), up());
	}

	Matrix4x4 RenderCamera::getProjMatrix() const
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


}

