#pragma once
#include  "core/math/vector2.h"
namespace QYHS
{
	
	enum class EditorCommand:unsigned int 
	{
		camera_left = 1 << 0,  // A
		camera_back = 1 << 1,  // S
		camera_foward = 1 << 2,  // W
		camera_right = 1 << 3,  // D
		camera_up = 1 << 4,  // Q
		camera_down = 1 << 5,  // E
		translation_mode = 1 << 6,  // T
		rotation_mode = 1 << 7,  // R
		scale_mode = 1 << 8,  // C
		exit = 1 << 9,  // Esc
		delete_object = 1 << 10, // Delete
	};

	class EditorInputManager
	{
	public:
		//EditorInputManager();
		void onKeyInEditorMode(int key, int scancode, int action, int mods);
		void registerInput();
		void initialize();
	public:
		void onScroll(double xoffset, double yoffset);
		void onKey(int key, int scancode, int action, int mods);
		void onMouseButton(int button, int action, int mods);
		void onCursorPos(float x_pos, float y_pos);
		void tick(float delta_time);
		void processEditorCommand(float delta_time);
		float m_mouse_x{ 0.0f };
		float m_mouse_y{ 0.0f };
		Vector2 m_engine_window_size{ 1280.f,960.f };
		float m_camera_speed{ 0.01f };
		unsigned int m_command{ 0 };
		unsigned int k_complement_control_command = 0xFFFFFFFF;
		bool m_first_initialize_mouse_pos = false;
		size_t m_selected_axis_by_cursor{ 3 };
	private:

	};
}