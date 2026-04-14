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

#define LIRUI_VERSION				"0.1.1-alpha"
#define LIRUI_VERSION_MAJOR			0
#define LIRUI_VERSION_MINOR			1
#define LIRUI_VERSION_PATCH			1
#define LIRUI_VERSION_TWEAK			0x0a			// 0x0a - alpha, 0x0b - beta, 0x0c - release candidate, 0x0f - release
#define LIRUI_VERSION_INT			((LIRUI_VERSION_MAJOR << 24) | (LIRUI_VERSION_MINOR << 16) | (LIRUI_VERSION_PATCH << 8) | LIRUI_VERSION_TWEAK)

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