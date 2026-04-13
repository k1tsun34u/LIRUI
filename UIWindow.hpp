#pragma once
#include "UIBasicWindow.hpp"

namespace LIR {
	namespace UI {
		class Window : public BasicWindow {
		public:
			Window(
				BasicWindow* parent,
				const std::wstring title,
				int x, int y, int width, int height
			);
		};
	}
}