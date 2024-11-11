#include "window_system.h"
#include "core/base/macro.h"
#include <iostream>

static void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*auto engine = reinterpret_cast<QyhsEngine*>(glfwGetWindowUserPointer(window));*/

}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	/*auto engine = reinterpret_cast<QyhsEngine*>(glfwGetWindowUserPointer(window));
	engine->framebufferResized = true;*/
}

namespace QYHS
{
	WindowSystem::~WindowSystem()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void WindowSystem::initialize(WindowCreateInfo* window_create_info)
	{
		if (!glfwInit())
		{
			//LOG_FATAL(__FUNCTION__, "failed to initialize GLFW!");
			std::cout << "failed to initialize GLFW!" << std::endl;
			return;
		}
		m_width = window_create_info->width;
		m_height = window_create_info->height;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_window = glfwCreateWindow(window_create_info->width, window_create_info->height, window_create_info->title, nullptr, nullptr);
		if (!m_window)
		{
			std::cout << "Failed to create window!" << std::endl;
			glfwTerminate();
			return;
		}

		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
		glfwSetKeyCallback(m_window, onKeyCallback);
		glfwSetCursorPosCallback(m_window, onMouseScrollCallback);
		glfwSetMouseButtonCallback(m_window, onMouseButtonCallback);
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetScrollCallback(m_window, onScrollCallback);

	}

	void WindowSystem::pollEvents()
	{
		glfwPollEvents();
	}

	bool WindowSystem::isMouseButton(int button) const
	{
		if (!m_window || button<GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
		{
			return false;
		}
		return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
	}

	void WindowSystem::hideCursor(bool is_hidden)
	{
		if (is_hidden)
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void WindowSystem::disableCursor(bool is_disabled)
	{
		if (is_disabled)
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

}




