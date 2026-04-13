#include "UIButton.hpp"

LIR::UI::Button::Button(
	BasicWindow* parent,
	const std::wstring title,
	int x, int y, int width, int height
) : Control(
	parent,
	title,
	x, y, width, height,
	ClassName(),
	WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0
) {}