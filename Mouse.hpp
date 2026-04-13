#pragma once
#include "Typedefs.hpp"

namespace LIR {
	namespace Mouse {
		inline POINT GetPos(LPARAM lParam) {
			return {
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam)
			};
		}
	}
}