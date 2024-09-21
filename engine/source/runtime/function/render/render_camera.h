#pragma once
#include <glm/glm.hpp>
namespace QYHS
{
	class RenderCamera
	{
	public:
		RenderCamera();
		void zoom(float offset);
		void initialize();
		void updateCursorPos(float x_pos, float y_pos);
		float getCameraSpeed() { return camera_speed; }
		glm::vec3 getCameraFront() { return camera_front; }
		glm::vec3 getCameraUp() { return camera_up; }
		void updateCameraOffset(glm::vec3 offset) { camera_pos_offset = offset; }
		void updateCameraPosition();
		glm::vec3 getCameraPos() { return camera_pos; }
		float getCameraFOV() { return m_fov; }
		void setCameraPosition(glm::vec3 pos) { this->camera_pos = pos; }
	private:
		float m_fov{ 45.f };

		glm::vec3 camera_pos = { 0.f,0.f,0.f };
		glm::vec3 camera_pos_offset = { 0.f,0.f,0.f };
		glm::vec3 camera_target = { 0.0f,0.0f,0.0f };
		glm::vec3 camera_front = { 0.f,0.f,-1.f };
		glm::vec3 camera_up = { 0.0f,1.0f,0.0f };
		float camera_speed = 0.1f;

		bool first_mouse = true;

		//Euler angle
		float pitch = 0.f;
		float yaw = -90.f;
		float roll = 0.f;

		float last_camera_x_pos;
		float last_camera_y_pos;

		float camera_fov{ 45.f };
	};
}