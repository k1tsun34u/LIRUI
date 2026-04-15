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
	WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 0
) {
	OnClick += [this](ClickEventArgs& args) -> EventResult {
		ToggleState();
		return EventResult{};
	};

	OnKeyDown += [this](KeyDownEventArgs& args) -> EventResult {
		if (
			args.Key == VK_SPACE ||
			args.Key == VK_RETURN
		) ToggleState();
		return EventResult{};
	};
}

bool LIR::UI::CheckBox::IsChecked() const {
	return _isChecked;
}

void LIR::UI::CheckBox::SetChecked(bool checked) {
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

void LIR::UI::CheckBox::ToggleState() {
	SetChecked(!_isChecked);
}

void LIR::UI::CheckBox::AfterDefaultProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (_renderMode != RenderMode::Native) return;

	if (uMsg == BM_SETCHECK) _isChecked = wParam == BST_CHECKED;
}