#pragma once
#include "Typedefs.hpp"

namespace LIR {
	namespace UI {
		struct EventResult {
			bool			Handled = false;			// stop other handlers
			bool			AllowDefault = true;		// allow DefWindowProc/DefSubclassProc to be called
		};
	}
}