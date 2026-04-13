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
	}
}