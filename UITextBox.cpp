#include "UITextBox.hpp"

LIR::UI::TextBox::TextBox(
	BasicWindow* parent,
	const std::wstring text,
	int x, int y, int width, int height
) : Control(
	parent,
	text,
	x, y, width, height,
	ClassName(),
	WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 0
) {
	SetText(text);

	OnInput += [this](InputEventArgs& args) -> EventResult {
		HandleInput(args);
		return EventResult{};
	};
}

std::wstring LIR::UI::TextBox::GetText() const {
	return _text;
}

void LIR::UI::TextBox::SetText(const std::wstring text) {
	Dispatcher::Invoke<void>([this, text]() {
		if (_text == text) return;

		_text = text;
		if (IsValid()) {
			HWND hWnd = _handle;
			if (_renderMode == RenderMode::Native) SetWindowTextW(hWnd, _text.c_str());
			InvalidateRect(hWnd, NULL, TRUE);
		}
	});
}

void LIR::UI::TextBox::HandleInput(InputEventArgs& args) {
	if (!IsValid()) return;

	if (_renderMode != RenderMode::Native) {
		if (args.Char == L'\b') {
			if (!_text.empty()) {
				wchar_t current = _text.back();
				_text.pop_back();
				if (current == L'\n' && !_text.empty() && _text.back() == L'\r') _text.pop_back();
			}
		}
		else if (args.Char == L'\r') _text += L"\r\n";
		else if (args.Char >= 32) _text.push_back(args.Char);

		InvalidateRect(_handle, NULL, TRUE);
	}
	else _text = GetTitle(); // TODO: fix this (event before DefWindowProc/DefSubclassProc => no current char)
}