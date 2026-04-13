#include "Typedefs.hpp"
#include "UIDispatcher.hpp"

namespace LIR {
	namespace UI {
		template<typename... Args>
		class EventHandlerArray {
		public:
			using Handler = std::function<void(const Args&...)>;
		
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
			
			void Invoke(const Args&... args) {
				if (!Dispatcher::IsUIThread()) {
					auto tuple = std::make_tuple(std::forward<Args>(args)...);
					Dispatcher::Post([this, tuple]() {
						std::apply([this](auto&&... unpacked) {
							Invoke(std::forward<decltype(unpacked)>(unpacked)...);
						}, tuple);
					});

					return;
				}

				auto handlers = _handlers;
				for (auto& e : handlers) e.handler(args...);
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
			
			// TODO: not safe
			size_t					_nextId = 1;
		};
	}
}