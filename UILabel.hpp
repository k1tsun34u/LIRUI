#pragma once
#include "UIControl.hpp"

namespace LIR {
	namespace UI {
		class Label : public Control {
		public:
			Label(
				BasicWindow* parent,
				const std::wstring title,
				int x, int y, int width, int height
			);

			void OnPaint(HDC hDC) override {
				if (_renderer) _renderer->DrawLabel(this, hDC);
			}

			const wchar_t* ClassName() const override {
				return L"STATIC";
			}
		};
	}
}