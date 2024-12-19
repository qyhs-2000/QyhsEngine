#pragma once
#include <Windows.h>
namespace QYHS
{
	class Canvas
	{
	public:
		int width{ 0 };
		int height{ 0 };
		float dpi{ 96 };
		void init(HWND hwnd);
		int getPhysicalWidth() { return width; }
		int getPhysicalHeight() { return height; }
	private:
	};
}