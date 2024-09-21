#pragma once

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
	private:
	};
}