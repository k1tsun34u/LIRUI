#include "UITextBox.hpp"

LIR::UI::TextBox::TextBox(
	BasicWindow* parent,
	const std::wstring title,
	int x, int y, int width, int height
) : Control(
	parent,
	title,
	x, y, width, height,
	ClassName(),
	WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | SS_NOTIFY, 0
) {}