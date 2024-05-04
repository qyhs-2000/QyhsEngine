#include "include/editor_input_manager.h"
#include "editor_global_context.h"
#include <function/global/global_context.h>
#include <function/render/window_system.h>
#include "editor_input_manager.h"
#include "editor_scene_manager.h"
#include <glfw/glfw3.h>
#include <iostream>
namespace QYHS
{
	/*EditorInputManager::EditorInputManager()
	{
		initialize();
	}*/
	void EditorInputManager::onKeyInEditorMode(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_Q:
				std::cout << "Q" << std::endl;
				break;
			case GLFW_KEY_E:
				std::cout << "E" << std::endl;
				break;
			case GLFW_KEY_W:
				std::cout << "Q" << std::endl;
				break;
			case GLFW_KEY_S:
				std::cout << "Q" << std::endl;
				break;
			case GLFW_KEY_A:
				std::cout << "Q" << std::endl;
				break;
			case GLFW_KEY_D:
				std::cout << "Q" << std::endl;
				break;
			default:
				break;
			}
		}
	}

	void EditorInputManager::registerInput()
	{
		g_runtime_global_context.m_window->registerOnScrollFunc(std::bind(&EditorInputManager::onScroll, this, std::placeholders::_1, std::placeholders::_2));
		g_runtime_global_context.m_window->registerOnKeyFunc(std::bind(&EditorInputManager::onKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		g_runtime_global_context.m_window->registerOnCursorPos(std::bind(&EditorInputManager::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
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
		glm::vec3 offset = { 0.f,0.f,0.f };
		if (action == GLFW_PRESS)
		{
			float speed = g_editor_global_context.m_scene_manager->getEditorCamera()->getCameraSpeed();
			glm::vec3 front = g_editor_global_context.m_scene_manager->getEditorCamera()->getCameraFront();
			glm::vec3 up = g_editor_global_context.m_scene_manager->getEditorCamera()->getCameraUp();

			switch (key)
			{
			case GLFW_KEY_W:
				offset = speed * front * delta_time;
				std::cout << "W";
				break;
			case GLFW_KEY_S:
				offset = -speed * front * delta_time;
				std::cout << "S";
				break;
			case GLFW_KEY_A:
				offset = -speed * glm::normalize(glm::cross(front, up)) * delta_time;
				std::cout << "A";
				break;
			case GLFW_KEY_D:
				offset = speed * glm::normalize(glm::cross(front, up)) * delta_time;
				std::cout << "D";
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
				offset = { 0.f,0.f,0.f };
				std::cout << "W";
				break;
			case GLFW_KEY_S:
				offset = { 0.f,0.f,0.f };
				std::cout << "S";
				break;
			case GLFW_KEY_A:
				offset = { 0.f,0.f,0.f };
				std::cout << "A";
				break;
			case GLFW_KEY_D:
				offset = { 0.f,0.f,0.f };
				std::cout << "D";
				break;
			default:
				break;
			}

		}
		g_editor_global_context.m_scene_manager->getEditorCamera()->updateCameraOffset(offset);
	}

	void EditorInputManager::onCursorPos(float x_pos, float y_pos)
	{
		g_editor_global_context.m_scene_manager->getEditorCamera()->updateCursorPos(x_pos, y_pos);

	}


}
