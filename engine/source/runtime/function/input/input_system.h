#pragma once
#include "core/math/math.h"
namespace QYHS
{
	enum class GameCommand:unsigned int 
	{
		left = 1 << 0,  // A
		back = 1 << 1,  // S
		foward = 1 << 2,  // W
		right = 1 << 3,  // D
	};

	class InputSystem
	{
		enum class InputCommand :unsigned int
		{
			forward = 1 << 0,
			back = 1 << 1,
			left = 1 << 2,
			right = 1 << 3
		};
	public:
		void initialize();
		void calculateCursorDeltaAngle();
		void onCursorMove(float pos_x, float pos_y);
		void onKeyFunction(int key, int scancode, int action, int mods);
		void tick(float delta_time);
		unsigned int getGameCommand() { return m_game_command; }
		Radian m_cursor_delta_yaw{ 0 };
		Radian m_cursor_delta_pitch{ 0 };
	private:
		float m_last_cursor_x;
		float m_last_cursor_y;
		bool last_cursor_initialize{ false };
		float m_cursor_delta_x{ 0.f };
		float m_cursor_delta_y{ 0.f };
		unsigned int k_complement_control_command = 0xFFFFFFFF;
		unsigned int m_game_command{ 0 };
	};
}