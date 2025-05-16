#pragma once
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#define PLATFORM_DESKTOP_WINDOWS
#endif // _WIN32

#define LoadLibrary(library) LoadLibraryA(library)
#define myGetProcAddress(handle,name)  GetProcAddress(handle,name)

namespace qyhs
{
	namespace platform
	{
#ifdef _WIN32
		using WindowType = HWND;
#else
		using WindowType = void*;
#endif // _WIN32

		struct WindowProperties
		{
			int width = 0;
			int height = 0;
			float dpi = 96;
		};

		void getWindowProperties(WindowType window, WindowProperties* window_properties);
	}
}