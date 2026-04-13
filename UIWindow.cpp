#include "UIWindow.hpp"

LIR::UI::Window::Window(
	BasicWindow* parent,
	const std::wstring title,
	int x, int y, int width, int height
) : BasicWindow(
	parent,
	title,
	x, y, width, height,
	false, RootClass,
	WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW
) {}