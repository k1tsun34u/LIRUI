#pragma once
#include "UIControl.hpp"

namespace LIR {
	namespace UI {
		class TextBox : public Control {
		public:
			TextBox(
				BasicWindow* parent,
				const std::wstring title,
				int x, int y, int width, int height
			);

			void OnPaint(HDC hDC) override {
				if (_renderer) _renderer->DrawTextBox(this, hDC);
			}

			const wchar_t* ClassName() const override {
				return L"EDIT";
			}
		};
	}
}