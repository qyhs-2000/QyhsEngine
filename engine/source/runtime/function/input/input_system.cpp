#include "input_system.h"
#include <core/math/math.h>
#include "engine.h"
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
		g_runtime_global_context.m_window_system->registerOnKeyFunc(std::bind(&InputSystem::onKeyFunction, this, std::placeholders::_1,
			std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	void InputSystem::tick(float delta_time)
	{
		if (!g_runtime_global_context.m_window_system->getFocusMode()) return;
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
	}

	void InputSystem::onKeyFunction(int key, int scancode, int action, int mods)
	{
		if (g_is_editor_mode) return;
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_W:
				m_game_command |= (unsigned int)GameCommand::foward;
				break;
			case GLFW_KEY_S:
				m_game_command |= (unsigned int)GameCommand::back;
				break;
			case GLFW_KEY_A:
				m_game_command |= (unsigned int)GameCommand::left;
				break;
			case GLFW_KEY_D:
				m_game_command |= (unsigned int)GameCommand::right;
				break;

			case GLFW_KEY_LEFT_ALT:
				g_runtime_global_context.m_window_system->toggleFocusMode();
				break;
			default:
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_W:
				m_game_command &= (k_complement_control_command^(unsigned int)GameCommand::foward);
				break;
			case GLFW_KEY_S:
				m_game_command &= (k_complement_control_command^(unsigned int)GameCommand::back);
				break;
			case GLFW_KEY_A:
				m_game_command &= (k_complement_control_command^(unsigned int)GameCommand::left);
				break;
			case GLFW_KEY_D:
				m_game_command &= (k_complement_control_command^(unsigned int)GameCommand::right);
				break;

			case GLFW_KEY_SPACE:
				m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::jump);
			default:
				break;
			}
		}
	}
}