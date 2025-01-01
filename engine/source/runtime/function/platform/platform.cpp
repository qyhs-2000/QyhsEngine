#include "platform.h"

namespace QYHS::platform
{
	void getWindowProperties(WindowType window, WindowProperties* window_properties)
	{
#if defined(PLATFORM_DESKTOP_WINDOWS)
		window_properties->dpi = GetDpiForWindow(window);
		RECT rect;
		GetClientRect(window, &rect);
		window_properties->width = int(rect.right - rect.left);
		window_properties->height = int(rect.bottom - rect.top);
#endif
	}
}