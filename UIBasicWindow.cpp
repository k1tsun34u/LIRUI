#include "UIBasicWindow.hpp"

std::once_flag				LIR::UI::BasicWindow::_registerClassFlag;
std::once_flag				LIR::UI::BasicWindow::_createThreadFlag;
bool						LIR::UI::BasicWindow::_rootClassRegistered = false;
// std::atomic<int>			LIR::UI::BasicWindow::_nextMenuIndex = 1000;
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
	if (!Dispatcher::Init()) return WorkResult::ErrorWindowWorkAsyncThreadNotCreated;

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
	// if (hWnd) PostMessage(hWnd, WM_CLOSE, 0, 0);
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
	return _rootClassRegistered = true;
}

void LIR::UI::BasicWindow::HandleMouseMove(UINT msg, WPARAM wParam, LPARAM lParam) {
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
		MouseEnter.Invoke(mouseEnterArgs);
	}

	MouseMoveEventArgs mouseMoveArgs = { this, time, pos };
	MouseMove.Invoke(mouseMoveArgs);

	if (_isLMBDown) {
		if (!_isDragging) {
			_isDragging = true;

			DragStartEventArgs dragStartArgs = { this, time, pos };
			DragStart.Invoke(dragStartArgs);
		}

		if (_isCaptured) {
			DragEventArgs dragArgs = { this, time, pos };
			Drag.Invoke(dragArgs);
		}
	}
}

void LIR::UI::BasicWindow::HandleMouseLeave(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_trackingMouseLeave = false;

	if (_isHovered) {
		_isHovered = false;

		MouseLeaveEventArgs mouseLeaveArgs = { this, time, pos };
		MouseLeave.Invoke(mouseLeaveArgs);
	}

	_isDragging = false;
	_isCaptured = false;
}

void LIR::UI::BasicWindow::HandleLMBDown(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_isLMBDown = true;
	_isCaptured = true;
	_lmbDownPos = pos;

	MouseDownEventArgs mouseDownArgs = { this, time, pos, Mouse::Button::Left };
	MouseDown.Invoke(mouseDownArgs);
}

void LIR::UI::BasicWindow::HandleLMBUp(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	bool wasPressed = _isLMBDown;
	_isLMBDown = false;
	_isCaptured = false;

	MouseUpEventArgs mouseUpArgs = { this, time, pos, Mouse::Button::Left };
	MouseUp.Invoke(mouseUpArgs);

	if (wasPressed && _isHovered) {
		ClickEventArgs mouseClickArgs = { this, time, pos, Mouse::Button::Left };
		Click.Invoke(mouseClickArgs);
	}

	if (_isDragging) {
		_isDragging = false;

		DragEndEventArgs dragEndArgs = { this, time, pos };
		DragEnd.Invoke(dragEndArgs);
	}
}

void LIR::UI::BasicWindow::HandleMMBDown(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_isMMBDown = true;

	MouseDownEventArgs mouseDownArgs = { this, time, pos, Mouse::Button::Middle };
	MouseDown.Invoke(mouseDownArgs);
}

void LIR::UI::BasicWindow::HandleMMBUp(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	bool wasPressed = _isMMBDown;
	_isMMBDown = false;

	MouseUpEventArgs mouseUpArgs = { this, time, pos, Mouse::Button::Middle };
	MouseUp.Invoke(mouseUpArgs);

	if (wasPressed && _isHovered) {
		ClickEventArgs mouseClickArgs = { this, time, pos, Mouse::Button::Middle };
		Click.Invoke(mouseClickArgs);
	}
}

void LIR::UI::BasicWindow::HandleRMBDown(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	_isRMBDown = true;

	MouseDownEventArgs mouseDownArgs = { this, time, pos, Mouse::Button::Right };
	MouseDown.Invoke(mouseDownArgs);
}

void LIR::UI::BasicWindow::HandleRMBUp(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	bool wasPressed = _isRMBDown;
	_isRMBDown = false;

	MouseUpEventArgs mouseUpArgs = { this, time, pos, Mouse::Button::Right };
	MouseUp.Invoke(mouseUpArgs);

	if (wasPressed && _isHovered) {
		ClickEventArgs mouseClickArgs = { this, time, pos, Mouse::Button::Right };
		Click.Invoke(mouseClickArgs);
	}
}

void LIR::UI::BasicWindow::HandleLMBDblClk(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	DblClkEventArgs dblClkArgs = { this, time, pos, Mouse::Button::Left };
	DblClk.Invoke(dblClkArgs);
}

void LIR::UI::BasicWindow::HandleMMBDblClk(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	DblClkEventArgs dblClkArgs = { this, time, pos, Mouse::Button::Middle };
	DblClk.Invoke(dblClkArgs);
}

void LIR::UI::BasicWindow::HandleRMBDblClk(UINT msg, WPARAM wParam, LPARAM lParam) {
	ULONGLONG time;
	POINT pos;
	GetMouseCtx(lParam, time, pos);

	DblClkEventArgs dblClkArgs = { this, time, pos, Mouse::Button::Right };
	DblClk.Invoke(dblClkArgs);
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

	if (window) {
		LRESULT res = 0;
		if (DispatchEvent(window, uMsg, wParam, lParam, &res)) return res;
	}

	if (uMsg == WM_NCDESTROY) {
		if (window) {
			window->_destroyed.store(true, std::memory_order_release);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LIR::UI::BasicWindow::GlobalSubclassProcedure(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData
) {
	BasicWindow* window = (BasicWindow*)dwRefData;

	if (window) {
		LRESULT res = 0;
		if (DispatchEvent(window, uMsg, wParam, lParam, &res)) return res;
	}

	if (uMsg == WM_NCDESTROY) {
		if (window) {
			window->_destroyed.store(true, std::memory_order_release);
			RemoveWindowSubclass(hWnd, GlobalSubclassProcedure, uIdSubclass);
		}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool LIR::UI::BasicWindow::DispatchEvent(
	BasicWindow* window,
	UINT uMsg,
	WPARAM wParam, LPARAM lParam,
	LRESULT* res
) {
	bool handled = false;
	switch (uMsg) {
	case WM_MOUSEMOVE: {
		window->HandleMouseMove(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_MOUSELEAVE: {
		window->HandleMouseLeave(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_LBUTTONDOWN: {
		window->HandleLMBDown(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_LBUTTONUP: {
		window->HandleLMBUp(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_MBUTTONDOWN: {
		window->HandleMMBDown(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_MBUTTONUP: {
		window->HandleMMBUp(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_RBUTTONDOWN: {
		window->HandleRMBDown(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_RBUTTONUP: {
		window->HandleRMBUp(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_LBUTTONDBLCLK: {
		window->HandleLMBDblClk(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_MBUTTONDBLCLK: {
		window->HandleMMBDblClk(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	case WM_RBUTTONDBLCLK: {
		window->HandleRMBDblClk(uMsg, wParam, lParam);
		handled = true;
		break;
	}
	}

	if (handled && res) *res = 0;
	return handled;
}