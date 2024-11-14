#include "input_system.h"
#include <core/math/math.h>
#include "function/global/global_context.h"
#include <core/math/vector2.h>
#include "function/render/window_system.h"
#include "function/render/render_system.h"
namespace QYHS
{
	void InputSystem::initialize()
	{
		
		g_runtime_global_context.m_window_system->registerOnCursorPos(std::bind(&InputSystem::onCursorMove, this,
			std::placeholders::_1, std::placeholders::_2));

	}

	void InputSystem::tick(float delta_time)
	{

		calculateCursorDeltaAngle();
		m_cursor_delta_x = 0;
		m_cursor_delta_y = 0;
	}

	void InputSystem::calculateCursorDeltaAngle()
	{
		Vector2 window_size = g_runtime_global_context.m_window_system->getWindowSize();
		std::shared_ptr<RenderCamera> main_render_camera = g_runtime_global_context.m_render_system->getRenderCamera();
		Vector2 fov = main_render_camera->getCameraFOV();

		Radian cursor_delta_yaw(Math::degreesToRadians(m_cursor_delta_x));
		Radian cursor_delta_pitch(Math::degreesToRadians(m_cursor_delta_y));
		
		m_cursor_delta_yaw = (cursor_delta_yaw / window_size[0]) * fov.x;
		
		m_cursor_delta_pitch = (cursor_delta_pitch / window_size[1]) * fov.y;
	}
	void InputSystem::onCursorMove(float pos_x, float pos_y)
	{
		if (!last_cursor_initialize)
		{
			m_last_cursor_x = pos_x;
			m_last_cursor_y = pos_y;
			last_cursor_initialize = true;
		}
		m_cursor_delta_x = pos_x - m_last_cursor_x;
		m_cursor_delta_y = pos_y - m_last_cursor_y;
		m_last_cursor_x = pos_x;
		m_last_cursor_y = pos_y;
		std::cout << m_cursor_delta_x << "  " << m_cursor_delta_y << std::endl;
	}
}