#include "UIControl.hpp"
#include "UIRadioGroup.hpp"

namespace LIR {
	namespace UI {
		class RadioButton : public Control {
		public:
			RadioButton(
				BasicWindow* parent,
				const std::wstring title,
				int x, int y, int width, int height,
				RadioGroup* group
			);

			~RadioButton();

			bool IsChecked() const;
			void SetChecked(bool checked);
			void Select();

			void OnPaint(HDC hDC) override {
				if (_renderer) _renderer->DrawRadioButton(this, hDC);
			}

			const wchar_t* ClassName() const override {
				return L"BUTTON";
			}

			WindowClass Class() const override {
				return WindowClass::RadioButton;
			}
		protected:
			bool							_isChecked = false;
			RadioGroup*						_group;

			void AfterDefaultProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		};
	}
}