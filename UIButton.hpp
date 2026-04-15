#pragma once
#include "UIControl.hpp"

namespace LIR {
	namespace UI {
		class Button : public Control {
		public:
			Button(
				BasicWindow* parent,
				const std::wstring title,
				int x, int y, int width, int height
			);

			void OnPaint(HDC hDC) override {
				if (_renderer) _renderer->DrawButton(this, hDC);
			}

			const wchar_t* ClassName() const override {
				return L"BUTTON";
			}

			WindowClass Class() const override {
				return WindowClass::Button;
			}
		};
	}
}