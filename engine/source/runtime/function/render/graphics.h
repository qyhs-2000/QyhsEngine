#pragma once

namespace QYHS
{
	enum class Format :uint8_t
	{
		R10G10B10A2_UNORM,
		B8G8R8A8_UNORM
	};

	enum class ColorSpace :uint8_t
	{
		SRGB
	};

	class Viewport
	{
	public:
		float width{ 0 };
		float height{ 0 };
		float top_left_x{ 0 };
		float top_left_y{ 0 };
		float min_depth{ 0 };
		float max_depth{ 0 };
	private:
	};	
}