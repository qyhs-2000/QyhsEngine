#pragma once
#include "core/math/math.h"
namespace QYHS
{
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
		void tick(float delta_time);
		Radian m_cursor_delta_yaw{ 0 };
		Radian m_cursor_delta_pitch{ 0 };
	private:
		float m_last_cursor_x;
		float m_last_cursor_y;
		bool last_cursor_initialize{ false };
		float m_cursor_delta_x{ 0.f };
		float m_cursor_delta_y{ 0.f };
	};
}