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
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetScrollCallback(m_window, onScrollCallback);

	}




	void WindowSystem::pollEvents()
	{
		glfwPollEvents();
	}

}



