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

			bool IsChecked() const;
			void SetChecked(bool checked);
			void ToggleState(void);

			void OnPaint(HDC hDC) override {
				if (_renderer) _renderer->DrawCheckBox(this, hDC);
			}

			const wchar_t* ClassName() const override {
				return L"BUTTON";
			}

			WindowClass Class() const override {
				return WindowClass::CheckBox;
			}
		protected:
			bool				_isChecked = false;

			void AfterDefaultProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		};
	}
}