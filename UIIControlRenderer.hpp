#pragma once
#include "Typedefs.hpp"

namespace LIR {
	namespace UI {
		class IControlRenderer {
		public:
			virtual ~IControlRenderer() = default;

			virtual void DrawButton(Button* button, HDC hDC) = 0;
			virtual void DrawLabel(Label* label, HDC hDC) = 0;
			virtual void DrawTextBox(TextBox* textBox, HDC hDC) = 0;
			virtual void DrawCheckBox(CheckBox* checkBox, HDC hDC) = 0;
		};
	}
}