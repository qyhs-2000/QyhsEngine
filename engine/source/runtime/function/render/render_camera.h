#pragma once
#include <glm/glm.hpp>
#include <core/math/vector2.h>
#include <core/math/vector3.h>
#include <core/math/matrix4.h>
#include <mutex>
namespace qyhs
{
	enum class RenderCameraType
	{
		Editor,
		Motor
	};

	class RenderCamera
	{
	public:
		RenderCamera();
		void zoom(float offset);
		void initialize();
		//void updateCursorPos(float x_pos, float y_pos);
		float getCameraSpeed() { return camera_speed; }
		Vector3 getCameraPos() const { return m_position; }
		float getCameraFOVY() { return m_fovy; }
		Vector2 getCameraFOV() { return Vector2(m_fovx, m_fovy); }
		void rotate(Vector2 delta);
		void move(Vector3 delta) { m_position += delta; }
		void setCameraPosition(Vector3 pos) { this->m_position = pos; };
		void changeType(RenderCameraType camera_type);
		Vector3 forward() const { return (m_invRotation * Y); }
		Vector3 right() const { return (m_invRotation * X); }
		Vector3 up() const { return (m_invRotation * Z); }
		Matrix4x4 getViewMatrix() ;
		Matrix4x4 getProjMatrix() ;
		void setAspect(float aspect);
		void lookAt(Vector3 position, Vector3 target, Vector3 up);
		Quaternion getRotation() const { return m_rotation; }
		void setMainViewMatrix(Matrix4x4 view_matrix);
		void setCurrentCameraType(RenderCameraType camera_type);
	public:
		static const Vector3 X, Y, Z;
		const Vector3 m_up_axis{ Z };
		Quaternion m_rotation{ Quaternion::IDENTITY };
		Quaternion m_invRotation{ Quaternion::IDENTITY };
		float m_fovx{ Degree(89.f).valueDegrees() };
		float m_fovy{ 0.f };
		float m_aspect{ 0.f };
		float m_znear{ 1000.f };
		float m_zfar{ 0.1f };
	protected:
		std::mutex m_view_matrix_mutex;
	private:
		static constexpr float MIN_FOV{ 10.0f };
		static constexpr float MAX_FOV{ 89.0f };
		Vector3 m_position = { 0.f,0.f,5.f };
		Vector3 camera_target = { 0.0f,0.0f,0.0f };
		Vector3 camera_front = { 0.f,0.f,-1.f };
		Vector3 camera_up = { 0.0f,1.0f,0.0f };
		float camera_speed = 0.1f;

		bool first_mouse = true;

		//Euler angle
		float pitch = 0.f;
		float yaw = -90.f;
		float roll = 0.f;

		float last_camera_x_pos;
		float last_camera_y_pos;

		float camera_fov{ 45.f };
		Matrix4x4 m_view_matrix{ Matrix4x4::IDENTITY };
		RenderCameraType m_current_camera_type{ RenderCameraType::Editor };
	};
	inline const Vector3 RenderCamera::X = { 1.0,0.0,0.0 };
	inline const Vector3 RenderCamera::Y = { 0.0,1.0,0.0 };
	inline const Vector3 RenderCamera::Z = { 0.0,0.0,1.0 };
}