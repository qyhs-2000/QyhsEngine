#include "canvas.h"

namespace QYHS
{
	void Canvas::init(HWND hwnd)
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		this->width = int(rect.right - rect.left);
		this->height = int(rect.bottom - rect.top);
		this->dpi = (float)GetDpiForWindow(hwnd);
	}
}