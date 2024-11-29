#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <functional>
#include "core//math/vector2.h"
namespace QYHS
{
	struct WindowCreateInfo
	{
		int width{ 1280 };
		int height{ 960 };
		const char* title{ "Qyhs" };
		bool is_fullscreen{ false };
	};

	class WindowSystem
	{
	public:
		typedef std::function<void(float, float)>		 onScrollFunc;
		typedef std::function<void(int, int, int, int)>  onKeyFunc;
		typedef std::function<void(int, int)>			 onCursorFunc;
		typedef std::function<void(int, int, int)>		 onMouseButtonFunc;
	public:
		~WindowSystem();
		void initialize(WindowCreateInfo* window_create_info);
		void pollEvents();
		void registerOnScrollFunc(onScrollFunc func) { m_onScrollFunc.push_back(func); }
		void registerOnKeyFunc(onKeyFunc func) { m_onKeyFunc.push_back(func); }
		void registerOnCursorPos(onCursorFunc func) { m_onCursorPosFunc.push_back(func); }
		void registerOnMouseButton(onMouseButtonFunc func) { m_onMouseButtonFunc.push_back(func); }
		void disableCursor(bool is_disabled);
		void toggleFocusMode() { 
			m_is_focus = !m_is_focus;
			glfwSetInputMode(m_window, GLFW_CURSOR, m_is_focus ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}
		void setFocusMode(bool focus_mode) {
			m_is_focus = focus_mode;
		}

		bool isMouseButton(int button)const;
		const int getWidth(){return m_width;}
		const int getHeight(){return m_height;}
		GLFWwindow* getWindow() { return m_window; }
		bool shouldCloseWindow() { return glfwWindowShouldClose(m_window); }
		Vector2 getWindowSize(){return Vector2(m_width, m_height);}
		bool getFocusMode() { return m_is_focus; }
	protected:
		static void onScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
		{
			WindowSystem* app = reinterpret_cast<WindowSystem*>(glfwGetWindowUserPointer(window));
			if(app)
			{ 
				app->onScroll(xoffset, yoffset);
			}
		}

		static void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowSystem* app = reinterpret_cast<WindowSystem*>(glfwGetWindowUserPointer(window));
			if (app)
			{
				app->onKey(key, scancode, action, mods);
			}
		}

		static void onMouseScrollCallback(GLFWwindow* window, double x_pos, double y_pos)
		{
			WindowSystem* app = reinterpret_cast<WindowSystem*>(glfwGetWindowUserPointer(window));
			if (app)
			{
				app->onCursorPos(x_pos,y_pos);
			}
			

		}

		static void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{
			WindowSystem* app = reinterpret_cast<WindowSystem*>(glfwGetWindowUserPointer(window));
			if (app)
			{
				app->onMouseButton(button,action,mods);
			}
		}

		void onScroll(float xoffset, float yoffset)
		{
			for (auto& func : m_onScrollFunc)
			{
				func(xoffset, yoffset);
			}
		}

		void onCursorPos(float x_pos, float y_pos)
		{
			for (auto& func : m_onCursorPosFunc)
			{
				func(x_pos, y_pos);
			}
		}

		void onMouseButton(int button, int action, int mods)
		{
			for (auto& func : m_onMouseButtonFunc)
			{
				func(button, action, mods);
			}
		}
	private:
		int m_width;
		int m_height;
		bool m_is_focus{ false };
		GLFWwindow* m_window{ nullptr };
		void onKey(int key, int scancode, int action, int mods)
		{
			for (auto& func : m_onKeyFunc)
			{
				func(key,scancode, action, mods);
			}
		}

		 static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			 WindowSystem* window_system = new WindowSystem();
			 if (window_system)
			 {
				 window_system->onKey(key, scancode,action,mods);
			 }
		}

		 std::vector<onScrollFunc>	m_onScrollFunc;
		 std::vector<onKeyFunc>     m_onKeyFunc;
		 std::vector< onCursorFunc> m_onCursorPosFunc;
		 std::vector<onMouseButtonFunc>	m_onMouseButtonFunc;
	};
	
}