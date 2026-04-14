#include "Typedefs.hpp"
#include "UIEventResult.hpp"
#include "UIDispatcher.hpp"

namespace LIR {
	namespace UI {
		template<typename... Args>
		class EventHandlerArray {
		public:
			using Handler = std::function<EventResult(Args&...)>;
		
			size_t Add(Handler handler) {
				if (!Dispatcher::IsUIThread()) return Dispatcher::Invoke<size_t>([this, handler = std::move(handler)]() mutable {
					return Add(handler);
				});

				_handlers.push_back({
					_nextId,
					std::move(handler)
				});

				return _nextId++;
			}
			
			void Remove(size_t id) {
				if (!Dispatcher::IsUIThread()) {
					Dispatcher::Invoke<void>([this, id]() {
						Remove(id);
					});

					return;
				}

				std::erase_if(_handlers, [&](auto& e) {
					return e.id == id;
				});
			}
			
			EventResult Invoke(Args&... args) {
				EventResult result;
				if (!Dispatcher::IsUIThread()) {
					auto tuple = std::make_tuple(std::forward<Args>(args)...);
					Dispatcher::Post([this, tuple]() mutable {
						std::apply([this](auto&&... unpacked) {
							Invoke(std::forward<decltype(unpacked)>(unpacked)...);
						}, tuple);
					});

					return result;
				}

				auto handlers = _handlers;
				for (auto& e : handlers) {
					EventResult tmp = e.handler(args...);
					if (!tmp.AllowDefault) result.AllowDefault = false;
					if (tmp.Handled) {
						result.Handled = true;
						break;
					}
				}

				return result;
			}

			template<typename F>
			size_t operator+=(F&& f) {
				return Add(Handler(std::forward<F>(f)));
			}

			void operator-=(size_t id) {
				Remove(id);
			}
		private:
			struct Entry {
				size_t				id;
				Handler				handler;
			};

			std::vector<Entry>		_handlers;
			
			// TODO: fix overflow (how?)
			size_t					_nextId = 1;
		};
	}
}