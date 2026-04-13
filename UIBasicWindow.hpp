#pragma once
#include "UIDispatcher.hpp"
#include "UIWorkResult.hpp"
#include "Mouse.hpp"
#include "MouseButton.hpp"
#include "UIEventArgs.hpp"
#include "UIEventHandlerArray.hpp"

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

			void AddChild(BasicWindow* window);
			BasicWindow* GetChild(size_t index) const;
			void RemoveChild(size_t index);
			void RemoveChild(BasicWindow* window);
			size_t GetNumChildren() const;

			EventHandlerArray<const MouseEnterEventArgs&>	MouseEnter;
			EventHandlerArray<const MouseLeaveEventArgs&>	MouseLeave;
			EventHandlerArray<const MouseMoveEventArgs&>	MouseMove;
			EventHandlerArray<const MouseDownEventArgs&>	MouseDown;
			EventHandlerArray<const MouseUpEventArgs&>		MouseUp;
			EventHandlerArray<const ClickEventArgs&>		Click;
			EventHandlerArray<const DblClkEventArgs&>		DblClk;
			EventHandlerArray<const DragStartEventArgs&>	DragStart;
			EventHandlerArray<const DragEventArgs&>			Drag;
			EventHandlerArray<const DragEndEventArgs&>		DragEnd;
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
			// static std::atomic<int>							_nextMenuIndex;
			static std::atomic<int>							_nextSubclassID;
			mutable std::shared_mutex						_eventHandlersMutex;
		private:
			WorkResult InitSync();
			static bool RegisterRootClass();

			void HandleMouseMove(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleLMBDown(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleLMBUp(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleMMBDown(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleMMBUp(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleRMBDown(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleRMBUp(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleLMBDblClk(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleMMBDblClk(UINT msg, WPARAM wParam, LPARAM lParam);
			void HandleRMBDblClk(UINT msg, WPARAM wParam, LPARAM lParam);

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

			static bool DispatchEvent(
				BasicWindow* window,
				UINT uMsg,
				WPARAM wParam, LPARAM lParam,
				LRESULT* res
			);
		};
	}
}