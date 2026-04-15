#pragma once
#include "UIBasicWindow.hpp"
#include "UIRenderMode.hpp"
#include "UIIControlRenderer.hpp"
#include "Mouse.hpp"
#include "MouseButton.hpp"

namespace LIR {
	namespace UI {
		class Control : public BasicWindow {
		public:
			Control(
				BasicWindow* parent,
				const std::wstring title,
				int x, int y, int width, int height,
				std::wstring clazz,
				DWORD styles = 0,
				DWORD exStyles = 0
			);

			void SetRenderer(std::unique_ptr<IControlRenderer> renderer);
			void SetRenderMode(RenderMode renderMode);
			std::unique_ptr<IControlRenderer>& GetRenderer();
			RenderMode GetRenderMode() const;

			virtual void OnPaint(HDC hDC) = 0;

			WindowClass Class() const override {
				return WindowClass::Control;
			}
		protected:

			RenderMode							_renderMode;
			std::unique_ptr<IControlRenderer>	_renderer;

			virtual const wchar_t* ClassName() const = 0;
		};
	}
}