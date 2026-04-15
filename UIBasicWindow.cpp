#include "UIBasicWindow.hpp"

std::once_flag				LIR::UI::BasicWindow::_registerClassFlag;
std::once_flag				LIR::UI::BasicWindow::_createThreadFlag;
bool						LIR::UI::BasicWindow::_rootClassRegistered = false;
std::atomic<int>			LIR::UI::BasicWindow::_nextSubclassID = 0;

LIR::UI::BasicWindow::BasicWindow(
	BasicWindow* parent,
	const std::wstring title,
	int x, int y, int width, int height,

	bool isControl,
	std::wstring clazz,
	DWORD styles,
	DWORD exStyles
) {
	_parent = parent;
	_title = title;
	_x = x;
	_y = y;
	_width = width;
	_height = height;

	_isControl = isControl;
	_class = clazz;
	_styles = styles;
	_exStyles = exStyles;

	_handle = nullptr;
	_destroyed.store(false, std::memory_order_release);
}

LIR::UI::BasicWindow::~BasicWindow() {
	if (IsValid()) {
		HWND hWnd = _handle;
		Dispatcher::Post([hWnd]() {
			if (IsWindow(hWnd)) PostMessage(hWnd, WM_CLOSE, 0, 0);
		});
	}
}

bool LIR::UI::BasicWindow::IsValid() const {
	HWND hWnd = _handle;
	return hWnd && !_destroyed.load(std::memory_order_acquire) && IsWindow(hWnd);
}

LIR::UI::BasicWindow* LIR::UI::BasicWindow::GetParent() {
	return _parent;
}

std::wstring LIR::UI::BasicWindow::GetTitle() const {
	if (!_handle) return _title;

	auto result = Dispatcher::Invoke<std::optional<std::wstring>>([this]() -> std::optional<std::wstring> {
		if (!IsValid()) return std::nullopt;

		int len = GetWindowTextLengthW(_handle);
		if (len <= 0) return L"";

		size_t size = (size_t)(len + 1);
		std::wstring buffer(size, L'\0');

		len = GetWindowTextW(_handle, buffer.data(), (int)size);
		if (len <= 0) return L"";

		buffer.resize(len);
		return buffer;
	});

	return result.value_or(L"");
}

bool LIR::UI::BasicWindow::GetPos(int& x, int& y) const {
	if (!_handle) {
		x = _x;
		y = _y;
		return true;
	}

	auto result = Dispatcher::Invoke<std::optional<POINT>>([this]() -> std::optional<POINT> {
		RECT rc = {};
		if (
			!IsValid() ||
			!GetWindowRect(_handle, &rc)
			) return std::nullopt;

		return POINT{ rc.left, rc.top };
	});

	if (!result) return false;

	x = (int)result->x;
	y = (int)result->y;
	return true;
}

bool LIR::UI::BasicWindow::GetSize(int& width, int& height) const {
	if (!_handle) {
		width = _width;
		height = _height;
		return true;
	}

	auto result = Dispatcher::Invoke<std::optional<SIZE>>([this]() -> std::optional<SIZE> {
		RECT rc = {};
		if (
			!IsValid() ||
			!GetWindowRect(_handle, &rc)
			) return std::nullopt;

		return SIZE{ rc.right - rc.left, rc.bottom - rc.top };
	});

	if (!result) return false;

	width = (int)result->cx;
	height = (int)result->cy;
	return true;
}

HWND LIR::UI::BasicWindow::GetHandle() const {
	return _handle;
}

std::wstring LIR::UI::BasicWindow::GetClass() const {
	if (!_handle) return _class;

	auto result = Dispatcher::Invoke<std::optional<std::wstring>>([this]() -> std::optional<std::wstring> {
		if (!IsValid()) return std::nullopt;

		std::wstring buffer(MAX_PATH + 1, L'\0');
		int len = GetClassNameW(_handle, buffer.data(), MAX_PATH);
		if (len <= 0) return L"";

		buffer.resize(len);
		return buffer;
	});

	return result.value_or(L"");
}

DWORD LIR::UI::BasicWindow::GetStyles() const {
	if (!_handle) return _styles;

	auto result = Dispatcher::Invoke<std::optional<DWORD>>([this]() -> std::optional<DWORD> {
		if (!IsValid()) return std::nullopt;

		return (DWORD)GetWindowLongPtr(_handle, GWL_STYLE);
	});

	return result.value_or(0);
}

DWORD LIR::UI::BasicWindow::GetExStyles() const {
	if (!_handle) return _exStyles;

	auto result = Dispatcher::Invoke<std::optional<DWORD>>([this]() -> std::optional<DWORD> {
		if (!IsValid()) return std::nullopt;

		return (DWORD)GetWindowLongPtr(_handle, GWL_EXSTYLE);
	});

	return result.value_or(0);
}

bool LIR::UI::BasicWindow::SetTitleSync(const std::wstring title) {
	if (!_handle) {
		_title = title;
		return true;
	}

	return Dispatcher::Invoke<bool>([this, title]() -> bool {
		if (!IsValid()) return false;
		return SetWindowTextW(_handle, title.c_str()) != FALSE;
	});
}

bool LIR::UI::BasicWindow::SetPosSync(int x, int y) {
	if (!_handle) {
		_x = x;
		_y = y;
		return true;
	}

	return Dispatcher::Invoke<bool>([this, x, y]() -> bool {
		if (!IsValid()) return false;
		return SetWindowPos(
			_handle, nullptr,
			x, y, 0, 0,
			SWP_NOOWNERZORDER | SWP_NOZORDER |
			SWP_NOSIZE
		) != FALSE;
	});
}

bool LIR::UI::BasicWindow::SetSizeSync(int width, int height) {
	if (!_handle) {
		_width = width;
		_height = height;
		return true;
	}

	return Dispatcher::Invoke<bool>([this, width, height]() -> bool {
		if (!IsValid()) return false;
		return SetWindowPos(
			_handle, nullptr,
			0, 0, width, height,
			SWP_NOOWNERZORDER | SWP_NOZORDER |
			SWP_NOMOVE
		) != FALSE;
	});
}

bool LIR::UI::BasicWindow::SetStylesSync(DWORD styles) {
	if (!_handle) {
		_styles = styles;
		return true;
	}

	return Dispatcher::Invoke<bool>([this, styles]() -> bool {
		if (!IsValid()) return false;

		SetLastError(0);
		LONG_PTR tmp = SetWindowLongPtr(
			_handle,
			GWL_STYLE,
			styles
		);

		if (!tmp && GetLastError() != 0) return false;
		return SetWindowPos(
			_handle, nullptr,
			0, 0, 0, 0,
			SWP_NOOWNERZORDER | SWP_NOZORDER |
			SWP_NOMOVE | SWP_NOSIZE |
			SWP_FRAMECHANGED
		) != FALSE;
	});
}

bool LIR::UI::BasicWindow::SetExStylesSync(DWORD exStyles) {
	if (!_handle) {
		_exStyles = exStyles;
		return true;
	}

	return Dispatcher::Invoke<bool>([this, exStyles]() -> bool {
		if (!IsValid()) return false;

		SetLastError(0);
		LONG_PTR tmp = SetWindowLongPtr(
			_handle,
			GWL_EXSTYLE,
			exStyles
		);

		if (!tmp && GetLastError() != 0) return false;
		return SetWindowPos(
			_handle, nullptr,
			0, 0, 0, 0,
			SWP_NOOWNERZORDER | SWP_NOZORDER |
			SWP_NOMOVE | SWP_NOSIZE |
			SWP_FRAMECHANGED
		) != FALSE;
	});
}

LIR::UI::WorkResult LIR::UI::BasicWindow::Work() {
	if (!Dispatcher::EnsureInitialized()) return WorkResult::ErrorWindowWorkAsyncThreadNotCreated;

	auto promise = std::make_shared<std::promise<WorkResult>>();
	auto future = promise->get_future();

	Dispatcher::Post([win = this, promise]() {
		promise->set_value(win->InitSync());
	});

	if (future.wait_for(
		std::chrono::seconds(5)) != std::future_status::ready
	) return WorkResult::ErrorTimeout;
	return future.get();
}

void LIR::UI::BasicWindow::Close() {
	if (!Dispatcher::IsUIThread()) {
		Dispatcher::Invoke<void>([this]() {
			Close();
		});

		return;
	}

	if (_destroyed.load()) return;

	HWND hWnd = _handle;
	if (hWnd) PostMessage(hWnd, WM_CLOSE, 0, 0);
}

void LIR::UI::BasicWindow::Destroy() {
	if (!Dispatcher::IsUIThread()) {
		Dispatcher::Invoke<void>([this]() {
			Destroy();
		});

		return;
	}

	if (_destroyed.load()) return;

	std::vector<BasicWindow*> children = _children;
	for (auto* child : children) if (child) child->Close();
	_children.clear();

	if (_parent) {
		auto& siblings = _parent->_children;
		std::erase(siblings, this);
		_parent = nullptr;
	}

	DestroyWindow(_handle);
	_handle = nullptr;
}

void LIR::UI::BasicWindow::GetMouseCtx(LPARAM lParam, ULONGLONG& time, POINT& pos) {
	time = GetTickCount64();
	pos = Mouse::GetPos(lParam);
}

void LIR::UI::BasicWindow::AddChild(BasicWindow* window) {
	if (!window) return;

	if (!Dispatcher::IsUIThread()) {
		Dispatcher::Invoke<void>([this, window]() {
			AddChild(window);
		});

		return;
	}

	if (std::find(
		_children.begin(), _children.end(), window
	) != _children.end()) return;

	_children.push_back(window);
	window->_parent = this;
}

LIR::UI::BasicWindow* LIR::UI::BasicWindow::GetChild(size_t index) const {
	if (!Dispatcher::IsUIThread()) {
		return Dispatcher::Invoke<BasicWindow*>([this, index]() -> BasicWindow* {
			return GetChild(index);
		});
	}

	size_t length = _children.size();
	if (index >= length) return nullptr;

	return _children[index];
}

void LIR::UI::BasicWindow::RemoveChild(size_t index) {
	if (!Dispatcher::IsUIThread()) {
		Dispatcher::Invoke<void>([this, index]() {
			RemoveChild(index);
		});

		return;
	}

	size_t length = _children.size();
	if (index >= length) return;

	_children.erase(_children.begin() + index);
}

void LIR::UI::BasicWindow::RemoveChild(BasicWindow* window) {
	if (!Dispatcher::IsUIThread()) {
		Dispatcher::Invoke<void>([this, window]() {
			RemoveChild(window);
		});

		return;
	}

	auto it = std::find(_children.begin(), _children.end(), window);
	if (it == _children.end()) return;

	(*it)->_parent = nullptr;
	_children.erase(it);
}

size_t LIR::UI::BasicWindow::GetNumChildren() const {
	if (!Dispatcher::IsUIThread()) {
		return Dispatcher::Invoke<size_t>([this]() -> size_t {
			return GetNumChildren();
		});
	}

	return _children.size();
}

LIR::UI::WorkResult LIR::UI::BasicWindow::InitSync() {
	std::call_once(_registerClassFlag, []() {RegisterRootClass();});
	if (!_rootClassRegistered) return WorkResult::ErrorRootClassNotRegistered;
	if (_handle) return WorkResult::ErrorAlreadyInitialized;

	_handle = CreateWindowExW(
		_exStyles,
		_class.c_str(), _title.c_str(),
		_styles,
		_x, _y, _width, _height,
		_parent && _parent->IsValid() ? _parent->_handle : nullptr,
		// (HMENU)(INT_PTR)(_isControl ? _nextMenuIndex++ : 0),
		nullptr,
		(HINSTANCE)GetModuleHandle(nullptr),
		(LPVOID)this
	);

	if (_handle == nullptr) return WorkResult::ErrorWindowNotCreated;

	if (_isControl) {
		if (!SetWindowSubclass(
			_handle,
			GlobalSubclassProcedure,
			++_nextSubclassID,
			(DWORD_PTR)this
		)) {
			DestroyWindow(_handle);
			_handle = nullptr;
			return WorkResult::ErrorSubclassNotSet;
		}
	}

	if (_parent) _parent->AddChild(this);

	ShowWindow(_handle, SW_SHOW);
	UpdateWindow(_handle);

	EventArgs createArgs = { this, GetTickCount64() };
	OnCreate.Invoke(createArgs);
	return WorkResult::Success;
}

bool LIR::UI::BasicWindow::RegisterRootClass() {
	HICON hIcon = LoadIcon(nullptr, IDI_APPLICATION);

	WNDCLASSEXW wcexw = {
		.cbSize = sizeof(wcexw),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = GlobalWindowProcedure,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = (HINSTANCE)GetModuleHandle(nullptr),
		.hIcon = hIcon,
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName = nullptr,
		.lpszClassName = RootClass,
		.hIconSm = hIcon
	};

	if (RegisterClassExW(&wcexw) == 0) return false;
	_rootClassRegistered = true;
	return _rootClassRegistered;
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleCreate(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	EventArgs eventArgs = { this, GetTickCount64() };
	return OnCreate.Invoke(eventArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleClose(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	EventArgs closeArgs = { this, GetTickCount64() };
	return OnClose.Invoke(closeArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	EventArgs eventArgs = { this, GetTickCount64() };
	return OnDestroy.Invoke(eventArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_mousePos = pos;

	if (!_trackingMouseLeave) {
		TRACKMOUSEEVENT tme{};
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetHandle();
		TrackMouseEvent(&tme);
		_trackingMouseLeave = true;
	}

	if (!_isHovered) {
		_isHovered = true;

		MouseEnterEventArgs mouseEnterArgs = { this, time, pos };
		OnMouseEnter.Invoke(mouseEnterArgs);
	}

	MouseMoveEventArgs mouseMoveArgs = { this, time, pos };
	EventResult res = OnMouseMove.Invoke(mouseMoveArgs);

	if (_isLMBDown) {
		if (!_isDragging) {
			_isDragging = true;

			DragStartEventArgs dragStartArgs = { this, time, pos };
			OnDragStart.Invoke(dragStartArgs);
		}

		if (_isCaptured) {
			DragEventArgs dragArgs = { this, time, pos };
			OnDrag.Invoke(dragArgs);
		}
	}

	return res;
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_trackingMouseLeave = false;

	EventResult res{};
	if (_isHovered) {
		_isHovered = false;

		MouseLeaveEventArgs mouseLeaveArgs = { this, time, pos };
		res = OnMouseLeave.Invoke(mouseLeaveArgs);
	}

	_isDragging = false;
	_isCaptured = false;
	return res;
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleLMBDown(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_isLMBDown = true;
	_isCaptured = true;
	_lmbDownPos = pos;

	MouseDownEventArgs mouseDownArgs = { this, time, pos, Mouse::Button::Left };
	return OnMouseDown.Invoke(mouseDownArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleLMBUp(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	bool wasPressed = _isLMBDown;
	_isLMBDown = false;
	_isCaptured = false;

	MouseUpEventArgs mouseUpArgs = { this, time, pos, Mouse::Button::Left };
	EventResult res = OnMouseUp.Invoke(mouseUpArgs);

	if (wasPressed && _isHovered) {
		ClickEventArgs mouseClickArgs = { this, time, pos, Mouse::Button::Left };
		OnClick.Invoke(mouseClickArgs);
	}

	if (_isDragging) {
		_isDragging = false;

		DragEndEventArgs dragEndArgs = { this, time, pos };
		OnDragEnd.Invoke(dragEndArgs);
	}

	return res;
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleMMBDown(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_isMMBDown = true;

	MouseDownEventArgs mouseDownArgs = { this, time, pos, Mouse::Button::Middle };
	return OnMouseDown.Invoke(mouseDownArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleMMBUp(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	bool wasPressed = _isMMBDown;
	_isMMBDown = false;

	MouseUpEventArgs mouseUpArgs = { this, time, pos, Mouse::Button::Middle };
	EventResult res = OnMouseUp.Invoke(mouseUpArgs);

	if (wasPressed && _isHovered) {
		ClickEventArgs mouseClickArgs = { this, time, pos, Mouse::Button::Middle };
		OnClick.Invoke(mouseClickArgs);
	}

	return res;
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleRMBDown(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_isRMBDown = true;

	MouseDownEventArgs mouseDownArgs = { this, time, pos, Mouse::Button::Right };
	return OnMouseDown.Invoke(mouseDownArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleRMBUp(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	bool wasPressed = _isRMBDown;
	_isRMBDown = false;

	MouseUpEventArgs mouseUpArgs = { this, time, pos, Mouse::Button::Right };
	EventResult res = OnMouseUp.Invoke(mouseUpArgs);

	if (wasPressed && _isHovered) {
		ClickEventArgs mouseClickArgs = { this, time, pos, Mouse::Button::Right };
		OnClick.Invoke(mouseClickArgs);
	}

	return res;
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleLMBDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	DblClkEventArgs dblClkArgs = { this, time, pos, Mouse::Button::Left };
	return OnDblClk.Invoke(dblClkArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleMMBDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	DblClkEventArgs dblClkArgs = { this, time, pos, Mouse::Button::Middle };
	return OnDblClk.Invoke(dblClkArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleRMBDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	DblClkEventArgs dblClkArgs = { this, time, pos, Mouse::Button::Right };
	return OnDblClk.Invoke(dblClkArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleFocus(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	EventArgs clickArgs = { this, GetTickCount64() };
	return OnFocus.Invoke(clickArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleFocusLose(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	EventArgs clickArgs = { this, GetTickCount64() };
	return OnFocusLose.Invoke(clickArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	KeyDownEventArgs keyDownArgs = {
		this, GetTickCount64(),
		(uint8_t)wParam,							// key
		(size_t)(lParam) & 0xffff,					// num presses while holding
		(uint8_t)(((size_t)(lParam) >> 16) & 0xff),	// scan code
		(bool)((((size_t)lParam) >> 24) & 1),		// is extended
		(bool)((((size_t)lParam) >> 30) & 1)		// was down before
	};

	return OnKeyDown.Invoke(keyDownArgs);
}
LIR::UI::EventResult LIR::UI::BasicWindow::HandleKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	KeyUpEventArgs keyUpArgs = {
		this, GetTickCount64(),
		(uint8_t)wParam,							// key
		(uint8_t)(((size_t)(lParam) >> 16) & 0xff),	// scan code
		(bool)((((size_t)lParam) >> 24) & 1)		// is extended
	};

	return OnKeyUp.Invoke(keyUpArgs);
}

LIR::UI::EventResult LIR::UI::BasicWindow::HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	InputEventArgs inputArgs = {
		this, GetTickCount64(),
		(wchar_t)wParam,							// char
		(size_t)(lParam) & 0xffff,					// num presses while holding
		(uint8_t)(((size_t)(lParam) >> 16) & 0xff),	// scan code
		(bool)((((size_t)lParam) >> 24) & 1),		// is extended
		(bool)((((size_t)lParam) >> 30) & 1)		// was down before
	};

	return OnInput.Invoke(inputArgs);
}

LRESULT CALLBACK LIR::UI::BasicWindow::GlobalWindowProcedure(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam, LPARAM lParam
) {
	BasicWindow* window = nullptr;
	if (uMsg == WM_CREATE) {
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		window = (BasicWindow*)cs->lpCreateParams;
		if (!window) return -1;

		SetLastError(0);
		LONG_PTR tmp = SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
		if (!tmp && GetLastError() != 0) return -1;
	}
	else window = (BasicWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	LRESULT res = ProcedureHelper(window, hWnd, uMsg, wParam, lParam, DefWindowProc);
	if (uMsg == WM_NCDESTROY) {
		if (window) window->_destroyed.store(true, std::memory_order_release);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
	}

	return res;
}

LRESULT CALLBACK LIR::UI::BasicWindow::GlobalSubclassProcedure(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData
) {
	BasicWindow* window = (BasicWindow*)dwRefData;
	LRESULT res = ProcedureHelper(window, hWnd, uMsg, wParam, lParam, DefSubclassProc);
	if (uMsg == WM_NCDESTROY) {
		if (window) window->_destroyed.store(true, std::memory_order_release);
		RemoveWindowSubclass(hWnd, GlobalSubclassProcedure, uIdSubclass);
	}

	return res;
}

LRESULT LIR::UI::BasicWindow::ProcedureHelper(
	BasicWindow* window,
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam, LPARAM lParam,
	WNDPROC proc
) {
	if (!window) return proc(hWnd, uMsg, wParam, lParam);

	window->BeforeDefaultProc(uMsg, wParam, lParam);

	LRESULT res = 0;
	EventResult eventRes = DispatchEvent(window, uMsg, wParam, lParam);
	if (eventRes.AllowDefault) res = proc(hWnd, uMsg, wParam, lParam);

	window->AfterDefaultProc(uMsg, wParam, lParam);
	return res;
}

LIR::UI::EventResult LIR::UI::BasicWindow::DispatchEvent(
	BasicWindow* window,
	UINT uMsg,
	WPARAM wParam, LPARAM lParam
) {
	switch (uMsg) {
	case WM_CLOSE: return window->HandleClose(uMsg, wParam, lParam);
	case WM_DESTROY: return window->HandleDestroy(uMsg, wParam, lParam);
	case WM_MOUSEMOVE: return window->HandleMouseMove(uMsg, wParam, lParam);
	case WM_MOUSELEAVE: return window->HandleMouseLeave(uMsg, wParam, lParam);
	case WM_LBUTTONDOWN: return window->HandleLMBDown(uMsg, wParam, lParam);
	case WM_LBUTTONUP: return window->HandleLMBUp(uMsg, wParam, lParam);
	case WM_MBUTTONDOWN: return window->HandleMMBDown(uMsg, wParam, lParam);
	case WM_MBUTTONUP: return window->HandleMMBUp(uMsg, wParam, lParam);
	case WM_RBUTTONDOWN: return window->HandleRMBDown(uMsg, wParam, lParam);
	case WM_RBUTTONUP: return window->HandleRMBUp(uMsg, wParam, lParam);
	case WM_LBUTTONDBLCLK: return window->HandleLMBDblClk(uMsg, wParam, lParam);
	case WM_MBUTTONDBLCLK: return window->HandleMMBDblClk(uMsg, wParam, lParam);
	case WM_RBUTTONDBLCLK: return window->HandleRMBDblClk(uMsg, wParam, lParam);
	case WM_SETFOCUS: return window->HandleFocus(uMsg, wParam, lParam);
	case WM_KILLFOCUS: return window->HandleFocusLose(uMsg, wParam, lParam);
	case WM_KEYDOWN: return window->HandleKeyDown(uMsg, wParam, lParam);
	case WM_KEYUP: return window->HandleKeyUp(uMsg, wParam, lParam);
	case WM_CHAR: return window->HandleInput(uMsg, wParam, lParam);
	default: return EventResult{};
	}
}