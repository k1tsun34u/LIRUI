#include "UIRadioButton.hpp"

LIR::UI::RadioButton::RadioButton(
	BasicWindow* parent,
	const std::wstring title,
	int x, int y, int width, int height,
	RadioGroup* group
) : Control(
	parent,
	title,
	x, y, width, height,
	ClassName(),
	WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 0
) {
	OnClick += [this](ClickEventArgs& args) -> EventResult {
		Select();
		return EventResult{};
	};

	OnKeyDown += [this](KeyDownEventArgs& args) -> EventResult {
		if (
			args.Key == VK_SPACE ||
			args.Key == VK_RETURN
		) Select();
		return EventResult{};
	};

	_group = group;
	if (_group) _group->Items.push_back(this);
}

LIR::UI::RadioButton::~RadioButton() {
	if (_group) std::erase(_group->Items, this);
}

bool LIR::UI::RadioButton::IsChecked() const {
	return _isChecked;
}

void LIR::UI::RadioButton::SetChecked(bool checked) {
	Dispatcher::Invoke<void>([this, checked]() {
		if (_isChecked == checked) return;
		_isChecked = checked;

		if (IsValid()) {
			HWND hWnd = _handle;
			if (_renderMode == RenderMode::Native) SendMessage(hWnd, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
			InvalidateRect(hWnd, NULL, TRUE);
		}
	});
}

void LIR::UI::RadioButton::Select() {
	if (_group) for (RadioButton* rb : _group->Items) {
		if (rb && rb != this) rb->SetChecked(false);
	}

	SetChecked(true);
}