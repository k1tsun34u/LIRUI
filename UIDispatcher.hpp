#pragma once
#include "Typedefs.hpp"

namespace LIR {
	namespace UI {
		class Dispatcher {
		public:
			static bool EnsureInitialized();
			static void Shutdown();

			static DWORD GetThreadID();

			static bool IsAlive();
			static bool IsUIThread();

			static bool Post(std::function<void()> task);

			template<typename T>
			static T Invoke(std::function<T()> func) {
				if (IsUIThread()) return func();

				auto promise = std::make_shared<std::promise<T>>();
				auto future = promise->get_future();

				Post([func, promise]() {
					try { promise->set_value(func()); }
					catch (...) { promise->set_exception(std::current_exception()); }
				});

				return future.get();
			}

			template<>
			static void Invoke(std::function<void()> func) {
				if (IsUIThread()) {
					func();
					return;
				}

				auto promise = std::make_shared<std::promise<void>>();
				auto future = promise->get_future();

				Post([func, promise]() {
					try {
						func();
						promise->set_value();
					}
					catch (...) { promise->set_exception(std::current_exception()); }
				});

				return future.get();
			}

			static void ProcessTasks();
		protected:
			static HANDLE								_threadHandle;
			static DWORD								_threadID;
			static std::promise<DWORD>					_threadIDPromise;
			static std::future<DWORD>					_threadIDFuture;

			static std::queue<std::function<void()>>	_tasks;
			static std::mutex							_tasksMutex;
		private:
			static DWORD WINAPI ThreadProc(LPVOID);
		};
	}
}