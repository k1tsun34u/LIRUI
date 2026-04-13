#pragma once
#include "Typedefs.hpp"

namespace LIR {
	namespace UI {
		class IControlRenderer {
		public:
			virtual ~IControlRenderer() = default;

			virtual void DrawButton(Button* button, HDC hDC) = 0;
		};
	}
}