#pragma once
#include "UIControl.hpp"

namespace LIR {
	namespace UI {
		class CheckBox : public Control {
		public:
			CheckBox(
				BasicWindow* parent,
				const std::wstring title,
				int x, int y, int width, int height
			);

			void OnPaint(HDC hDC) override {
				if (_renderer) _renderer->DrawCheckBox(this, hDC);
			}

			const wchar_t* ClassName() const override {
				return L"BUTTON";
			}
		};
	}
}