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

void LIR::UI::TextBox::AfterDefaultProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!IsValid()) return;

	if (_renderMode == RenderMode::Native) {
		switch (uMsg) {
		case WM_CHAR:
		case WM_PASTE:
		case WM_CUT:
		case WM_UNDO:
		case WM_KEYUP:
			std::wstring oldText = _text;
			std::wstring newText = GetTitle();
			if (oldText == newText) break;

			_text = newText;

			TextChangeEventArgs textChangeArgs = {
				this, GetTickCount64(),
				oldText, newText
			};

			OnTextChange.Invoke(textChangeArgs);
			break;
		}
	}
	else OutputDebugStringA("Custom TextBox message handler is not implemented yet!\n");
}