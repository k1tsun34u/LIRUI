#include "UILabel.hpp"

LIR::UI::Label::Label(
	BasicWindow* parent,
	const std::wstring title,
	int x, int y, int width, int height
) : Control(
	parent,
	title,
	x, y, width, height,
	WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_NOTIFY, 0
) {}