#pragma once
#include <string>
#include <mutex>
#include <queue>
#include <future>
#include <optional>
#include <functional>
#include <shared_mutex>

#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define WM_WAKEUP					(WM_APP + 1)

namespace LIR {
	namespace UI {
		enum class WindowClass;
		enum class WorkResult;
		class Dispatcher;

		struct EventArgs;
		struct MouseEnterEventArgs;
		struct MouseLeaveEventArgs;
		struct MouseMoveEventArgs;
		struct MouseDownEventArgs;
		struct MouseUpEventArgs;
		struct ClickEventArgs;
		struct DblClkEventArgs;
		struct DragEventArgs;
		struct DragStartEventArgs;
		struct DragEndEventArgs;

		template<typename... Args>
		class EventHandlerArray;

		class BasicWindow;
		typedef bool (*EventHandler)(BasicWindow* window, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* res);

		class Window;
		class Control;

		enum class RenderMode;
		class IControlRenderer;

		class Button;
		class Label;
		class TextBox;
		class CheckBox;
	}

	namespace Mouse {
		enum class Button;
	}
}