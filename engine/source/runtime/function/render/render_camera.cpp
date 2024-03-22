#include "render_camera.h"

QYHS::RenderCamera::RenderCamera()
{
	initialize();
}

void QYHS::RenderCamera::zoom(float offset)
{
	m_fov -= offset;
	if (m_fov < 1.0f)
		m_fov = 1.0f;
	if (m_fov > 80.0f)
		m_fov = 80.0f;
}

//TODO: initialize last camera pos

void QYHS::RenderCamera::initialize()
{
	last_camera_x_pos = 2500/2;
	last_camera_y_pos = 1600/2;
}

void QYHS::RenderCamera::updateCursorPos(float x_pos, float y_pos)
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
}

void QYHS::RenderCamera::updateCameraPosition()
{
	camera_pos += camera_pos_offset;
}
