#include "UICheckBox.hpp"

LIR::UI::CheckBox::CheckBox(
	BasicWindow* parent,
	const std::wstring title,
	int x, int y, int width, int height
) : Control(
	parent,
	title,
	x, y, width, height,
	ClassName(),
	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | SS_NOTIFY, 0
) {}