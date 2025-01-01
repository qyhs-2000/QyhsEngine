#pragma once
#include "function/platform/platform.h"
namespace QYHS
{
	class Canvas
	{
	public:
		inline void init(platform::WindowProperties window_properties)
		{
			width = window_properties.width;
			height = window_properties.height;
			dpi = window_properties.dpi;
		}

		inline void init(platform::WindowType window)
		{
			platform::WindowProperties window_properties;
			platform::getWindowProperties(window, &window_properties);

		}
	private:
		uint32_t width{ 0 };
		uint32_t height{ 0 };
		float dpi{ 0 };
	};
}