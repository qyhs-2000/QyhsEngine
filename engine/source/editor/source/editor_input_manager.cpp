#include "include/editor_input_manager.h"
#include "editor_global_context.h"
#include <function/global/global_context.h>
#include <function/render/window_system.h>
#include "editor_input_manager.h"
#include "editor_scene_manager.h"
#include "function/render/render_system.h"
#include <glfw/glfw3.h>
#include "core/math/vector2.h"
#include <iostream>
#include "function/render/render_system.h"
namespace qyhs
{

	void EditorInputManager::registerInput()
	{				
		g_runtime_global_context.m_window_system->registerOnScrollFunc(std::bind(&EditorInputManager::onScroll, this, std::placeholders::_1, std::placeholders::_2));
		g_runtime_global_context.m_window_system->registerOnKeyFunc(std::bind(&EditorInputManager::onKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		g_runtime_global_context.m_window_system->registerOnCursorPos(std::bind(&EditorInputManager::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
		g_runtime_global_context.m_window_system->registerOnMouseButton(std::bind(&EditorInputManager::onMouseButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	void EditorInputManager::initialize()
	{
		registerInput();
	}

	void EditorInputManager::onScroll(double xoffset, double yoffset)
	{
		g_editor_global_context.m_scene_manager->getEditorCamera()->zoom((float)yoffset);
	}

	void EditorInputManager::onKey(int key, int scancode, int action, int mods)
	{
		float delta_time = 0.05;
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_A:
				m_command |= (unsigned int)EditorCommand::camera_left;
				break;
			case GLFW_KEY_S:
				m_command |= (unsigned int)EditorCommand::camera_back;
				break;
			case GLFW_KEY_W:
				m_command |= (unsigned int)EditorCommand::camera_foward;
				break;
			case GLFW_KEY_D:
				m_command |= (unsigned int)EditorCommand::camera_right;
				break;
			case GLFW_KEY_E:
				m_command |= (unsigned int)EditorCommand::camera_up;
				break;
			case GLFW_KEY_Q:
				m_command |= (unsigned int)EditorCommand::camera_down;
				break;
			default:
				break;
			}

		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_A:
				m_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_left);
				break;
			case GLFW_KEY_S:
				m_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_back);
				break;
			case GLFW_KEY_W:
				m_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_foward);
				break;
			case GLFW_KEY_D:
				m_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_right);
				break;
			case GLFW_KEY_E:
				m_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_up);
				break;
			case GLFW_KEY_Q:
				m_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_down);
				break;
			case GLFW_KEY_DELETE:
				m_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::delete_object);
				break;
			default:
				break;
			}

		}
	}
	bool EditorInputManager::isCursorInRect(Vector2 rect_pos, Vector2 rect_size)
	{
		return rect_pos.x < m_mouse_x && m_mouse_x < rect_pos.x + rect_size.x && rect_pos.y < m_mouse_y && m_mouse_y < rect_pos.y + rect_size.y;
	}


	void EditorInputManager::onMouseButton(int button, int action, int mods)
	{
		if (m_selected_axis_by_cursor != 3) return;
		
		if (!isCursorInRect(m_engine_window_pos, m_engine_window_size)) return;
		if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			Vector2 picked_uv = Vector2((m_mouse_x - m_engine_window_pos.x) / m_engine_window_size.x, (m_mouse_y - m_engine_window_pos.y) / m_engine_window_size.y);
			size_t select_mesh_id = g_editor_global_context.m_scene_manager->getMeshIDByPickedUV(picked_uv);
			size_t select_gobject_id = g_editor_global_context.m_render_system->getGObjectIDByMeshID(select_mesh_id);
			g_editor_global_context.m_scene_manager->GObjectSelected(select_gobject_id);
		}
	}

	void EditorInputManager::onCursorPos(float x_pos, float y_pos)
	{
		if (!m_first_initialize_mouse_pos)
		{
			m_mouse_x = x_pos;
			m_mouse_y = y_pos;
			m_first_initialize_mouse_pos = true;
		}
		if (g_editor_global_context.m_window_system->isMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
		{
			b_control_camera = true;
			g_runtime_global_context.m_window_system->disableCursor(b_control_camera);
			g_editor_global_context.m_scene_manager->getEditorCamera()->rotate(Vector2(y_pos - m_mouse_y, x_pos - m_mouse_x)*0.14);
		}
		else if (g_editor_global_context.m_window_system->isMouseButton(GLFW_MOUSE_BUTTON_LEFT))
		{
			g_editor_global_context.m_scene_manager->moveObject(x_pos,y_pos,m_mouse_x,m_mouse_y,m_engine_window_size);
			g_runtime_global_context.m_window_system->disableCursor(false);
		}
		else
		{
			if (isCursorInRect(m_engine_window_pos, m_engine_window_size))
			{
				Vector2 cursor_uv = Vector2(((m_mouse_x - m_engine_window_pos.x) / m_engine_window_size.x), ((m_mouse_y -m_engine_window_pos.y)/ m_engine_window_size.y));
				m_selected_axis_by_cursor = g_editor_global_context.m_scene_manager->updateCursorOnAxis(cursor_uv,m_engine_window_size);
				b_control_camera = false;
				g_runtime_global_context.m_window_system->disableCursor(b_control_camera);
			}
		}
		m_mouse_x = x_pos;
		m_mouse_y = y_pos;

	}

	void EditorInputManager::tick(float delta_time)
	{
		processEditorCommand(delta_time);
	}

	void EditorInputManager::processEditorCommand(float delta_time)
	{
		std::shared_ptr<RenderCamera> camera = g_runtime_global_context.m_render_system->getRenderCamera();

		Quaternion camera_rotation = camera->getRotation().inverse();
		Vector3 camera_relative_position = { 0.f,0.f,0.f };
		if (m_command & (unsigned int)EditorCommand::camera_foward)
		{
			camera_relative_position += camera_rotation * Vector3{ 0.f,1.f,0.f } *m_camera_speed;
		}
		if (m_command & (unsigned int)EditorCommand::camera_back)
		{
			camera_relative_position += camera_rotation * Vector3{ 0.f,-1.f,0.f }*m_camera_speed;
		}
		if (m_command & (unsigned int)EditorCommand::camera_left)
		{
			camera_relative_position += camera_rotation * Vector3{ -1.f,0.f,0.f }*m_camera_speed;
		}
		if (m_command & (unsigned int)EditorCommand::camera_right)
		{
			camera_relative_position += camera_rotation * Vector3{ 1.f,0.f,0.f }*m_camera_speed;
		}
		if (m_command & (unsigned int)EditorCommand::camera_up)
		{
			camera_relative_position += Vector3{ 0.f,0.f,1.f }*m_camera_speed;
		}
		if (m_command & (unsigned int)EditorCommand::camera_down)
		{
			camera_relative_position += Vector3{ 0.f,0.f,-1.f }*m_camera_speed;
		}
		if (m_command & (unsigned int)EditorCommand::delete_object)
		{
			
		}
		if (b_control_camera)
		{
			camera->move(camera_relative_position);
		}
	}

}
