#pragma once
#include "UIControl.hpp"

namespace LIR {
	namespace UI {
		class TextBox : public Control {
		public:
			TextBox(
				BasicWindow* parent,
				const std::wstring text,
				int x, int y, int width, int height
			);

			std::wstring GetText() const;
			void SetText(const std::wstring text);

			EventHandlerArray<TextChangeEventArgs&>		OnTextChange;

			void OnPaint(HDC hDC) override {
				if (_renderer) _renderer->DrawTextBox(this, hDC);
			}

			const wchar_t* ClassName() const override {
				return L"EDIT";
			}

			WindowClass Class() const override {
				return WindowClass::TextBox;
			}
		protected:
			std::wstring								_text;

			void AfterDefaultProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		};
	}
}