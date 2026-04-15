#include "Typedefs.hpp"

namespace LIR {
	namespace UI {
		struct EventArgs {
			BasicWindow*			Sender;
			ULONGLONG				Time;
		};

		struct MouseEnterEventArgs : public EventArgs {
			POINT					EnterPos;
		};

		struct MouseLeaveEventArgs : public EventArgs {
			POINT					LeavePos;
		};

		struct MouseMoveEventArgs : public EventArgs {
			POINT					CurrentPos;
		};

		struct MouseDownEventArgs : public EventArgs {
			POINT					DownPos;
			Mouse::Button			Button;
		};

		struct MouseUpEventArgs : public EventArgs {
			POINT					UpPos;
			Mouse::Button			Button;
		};

		struct ClickEventArgs : public EventArgs {
			POINT					ClickPos;
			Mouse::Button			Button;
		};

		struct DblClkEventArgs : public EventArgs {
			POINT					DblClkPos;
			Mouse::Button			Button;
		};

		struct DragEventArgs : public EventArgs {
			POINT					CurrentPos;
		};

		struct DragStartEventArgs : public EventArgs {
			POINT					StartPos;
		};

		struct DragEndEventArgs : public EventArgs {
			POINT					EndPos;
		};

		struct KeyDownEventArgs : public EventArgs {
			uint8_t					Key;
			size_t					NumPressesWhileHolding;
			uint8_t					ScanCode;
			bool					IsExtended;
			bool					WasDown;
		};

		struct KeyUpEventArgs : public EventArgs {
			uint8_t					Key;
			uint8_t					ScanCode;
			bool					IsExtended;
		};
		
		struct InputEventArgs : public EventArgs {
			wchar_t					Char;
			size_t					NumPressesWhileHolding;
			uint8_t					ScanCode;
			bool					IsExtended;
			bool					WasDown;
		};


		struct TextChangeEventArgs : public EventArgs {
			std::wstring			OldText;
			std::wstring			NewText;
		};
	}
}