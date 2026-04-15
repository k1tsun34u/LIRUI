#pragma once
#include "UIDispatcher.hpp"
#include "UIWorkResult.hpp"
#include "Mouse.hpp"
#include "MouseButton.hpp"
#include "UIEventArgs.hpp"
#include "UIEventHandlerArray.hpp"
#include "UIWindowClass.hpp"

namespace LIR {
	namespace UI {
		class BasicWindow {
		public:
			static constexpr wchar_t						RootClass[] = L"LIR_UI_Window";

			BasicWindow(
				BasicWindow* parent,
				const std::wstring title,
				int x, int y, int width, int height,

				bool isControl,
				std::wstring clazz,
				DWORD styles,
				DWORD exStyles
			);

			virtual ~BasicWindow();

			bool IsValid() const;

			BasicWindow* GetParent();
			std::wstring GetTitle() const;
			bool GetPos(int& x, int& y) const;
			bool GetSize(int& width, int& height) const;
			HWND GetHandle() const;
			std::wstring GetClass() const;
			DWORD GetStyles() const;
			DWORD GetExStyles() const;

			bool SetTitleSync(const std::wstring title);
			bool SetPosSync(int x, int y);
			bool SetSizeSync(int width, int height);
			bool SetStylesSync(DWORD styles);
			bool SetExStylesSync(DWORD exStyles);

			WorkResult Work();
			void Close();
			void Destroy();

			void AddChild(BasicWindow* window);
			BasicWindow* GetChild(size_t index) const;
			void RemoveChild(size_t index);
			void RemoveChild(BasicWindow* window);
			size_t GetNumChildren() const;

			EventHandlerArray<EventArgs&>					OnCreate;
			EventHandlerArray<EventArgs&>					OnClose;
			EventHandlerArray<EventArgs&>					OnDestroy;
			EventHandlerArray<MouseEnterEventArgs&>			OnMouseEnter;
			EventHandlerArray<MouseLeaveEventArgs&>			OnMouseLeave;
			EventHandlerArray<MouseMoveEventArgs&>			OnMouseMove;
			EventHandlerArray<MouseDownEventArgs&>			OnMouseDown;
			EventHandlerArray<MouseUpEventArgs&>			OnMouseUp;
			EventHandlerArray<ClickEventArgs&>				OnClick;
			EventHandlerArray<DblClkEventArgs&>				OnDblClk;
			EventHandlerArray<DragStartEventArgs&>			OnDragStart;
			EventHandlerArray<DragEventArgs&>				OnDrag;
			EventHandlerArray<DragEndEventArgs&>			OnDragEnd;
			EventHandlerArray<EventArgs&>					OnFocus;
			EventHandlerArray<EventArgs&>					OnFocusLose;
			EventHandlerArray<KeyDownEventArgs&>			OnKeyDown;
			EventHandlerArray<KeyUpEventArgs&>				OnKeyUp;
			EventHandlerArray<InputEventArgs&>				OnInput;

			virtual WindowClass Class() const {
				return WindowClass::BasicWindow;
			}
		protected:
			void GetMouseCtx(LPARAM lParam, ULONGLONG& time, POINT& pos);

			BasicWindow*									_parent;
			std::wstring									_title;
			int												_x;
			int												_y;
			int												_width;
			int												_height;

			bool											_isControl;
			std::wstring									_class;
			DWORD											_styles;
			DWORD											_exStyles;

			HWND											_handle = nullptr;
			std::atomic<bool>								_destroyed = false;

			bool											_isHovered = false;
			bool											_isFocused = false;
			bool											_isCaptured = false;
			bool											_isDragging = false;
			bool											_isLMBDown = false;
			bool											_isMMBDown = false;
			bool											_isRMBDown = false;
			bool											_trackingMouseLeave = false;
			POINT											_lmbDownPos = {};
			POINT											_mousePos = {};

			std::vector<BasicWindow*>						_children;

			static std::once_flag							_registerClassFlag;
			static std::once_flag							_createThreadFlag;
			static bool										_rootClassRegistered;
			static std::atomic<int>							_nextSubclassID;
			mutable std::shared_mutex						_eventHandlersMutex;

			virtual void BeforeDefaultProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {}
			virtual void AfterDefaultProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {}
		private:
			WorkResult InitSync();
			static bool RegisterRootClass();

			EventResult HandleCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleLMBDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleLMBUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleMMBDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleMMBUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleRMBDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleRMBUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleLMBDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleMMBDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleRMBDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleFocus(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleFocusLose(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
			EventResult HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam);

			static LRESULT CALLBACK GlobalWindowProcedure(
				HWND hWnd,
				UINT uMsg,
				WPARAM wParam, LPARAM lParam
			);

			static LRESULT CALLBACK GlobalSubclassProcedure(
				HWND hWnd,
				UINT uMsg,
				WPARAM wParam, LPARAM lParam,
				UINT_PTR uIdSubclass,
				DWORD_PTR dwRefData
			);

			static LRESULT ProcedureHelper(
				BasicWindow* window,
				HWND hWnd,
				UINT uMsg,
				WPARAM wParam, LPARAM lParam,
				WNDPROC proc
			);

			static EventResult DispatchEvent(
				BasicWindow* window,
				UINT uMsg,
				WPARAM wParam, LPARAM lParam
			);
		};
	}
}