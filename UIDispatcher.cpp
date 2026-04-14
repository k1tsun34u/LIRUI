#include "UIDispatcher.hpp"

HANDLE LIR::UI::Dispatcher::_threadHandle = nullptr;
DWORD LIR::UI::Dispatcher::_threadID = 0;
std::promise<DWORD> LIR::UI::Dispatcher::_threadIDPromise;
std::future<DWORD> LIR::UI::Dispatcher::_threadIDFuture;

std::queue<std::function<void()>> LIR::UI::Dispatcher::_tasks;
std::mutex LIR::UI::Dispatcher::_tasksMutex;

bool LIR::UI::Dispatcher::EnsureInitialized() {
	if (IsAlive()) return true;

	_threadIDPromise = std::promise<DWORD>();
	_threadIDFuture = _threadIDPromise.get_future();
	_threadHandle = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
	if (!_threadHandle) return false;

	DWORD id = _threadIDFuture.get();
	return id != 0;
}

void LIR::UI::Dispatcher::Shutdown() {
	if (!IsAlive()) return;

	PostThreadMessage(_threadID, WM_QUIT, 0, 0);
	WaitForSingleObject(_threadHandle, 5000);

	CloseHandle(_threadHandle);
	_threadHandle = nullptr;
	_threadID = 0;
}

DWORD LIR::UI::Dispatcher::GetThreadID() {
	return _threadID;
}

bool LIR::UI::Dispatcher::IsAlive() {
	return _threadHandle != nullptr;
}

bool LIR::UI::Dispatcher::IsUIThread() {
	if (!IsAlive()) return false;
	return GetCurrentThreadId() == _threadID;
}

bool LIR::UI::Dispatcher::Post(std::function<void()> task) {
	if (!EnsureInitialized()) return false;

	if (IsUIThread()) {
		task();
		return true;
	}

	{
		std::lock_guard<std::mutex> lock(_tasksMutex);
		_tasks.push(std::move(task));
	}

	PostThreadMessage(_threadID, WM_WAKEUP, 0, 0);
	return true;
}

void LIR::UI::Dispatcher::ProcessTasks() {
	std::queue<std::function<void()>> local;

	{
		std::lock_guard<std::mutex> lock(_tasksMutex);
		if (_tasks.empty()) return;

		std::swap(local, _tasks);
	}

	while (!local.empty()) {
		try { local.front()(); }
		catch (...) { OutputDebugStringA("Dispatcher task exception\n"); }
		local.pop();
	}
}

DWORD WINAPI LIR::UI::Dispatcher::ThreadProc(LPVOID) {
	MSG msg;
	PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);

	_threadID = GetCurrentThreadId();
	_threadIDPromise.set_value(_threadID);

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (msg.message == WM_WAKEUP) {
			ProcessTasks();
			continue;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return ERROR_SUCCESS;
}