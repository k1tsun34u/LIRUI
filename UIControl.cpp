#include "UIControl.hpp"

LIR::UI::Control::Control(
	BasicWindow* parent,
	const std::wstring title,
	int x, int y, int width, int height,
	std::wstring clazz,
	DWORD styles, DWORD exStyles
) : BasicWindow(
	parent,
	title,
	x, y, width, height,
	true, clazz,
	styles | WS_CHILD | WS_VISIBLE,
	exStyles
){
	_renderMode = RenderMode::Native;
	_isHovered = _isFocused = _isCaptured = false;
	_isLMBDown = _isMMBDown = _isRMBDown = false;
	_isDragging = false;
	_lmbDownPos = _mousePos = {};
	_trackingMouseLeave = false;
}

void LIR::UI::Control::SetRenderer(std::unique_ptr<IControlRenderer> renderer) {
	_renderer = std::move(renderer);
	if (IsValid()) InvalidateRect(GetHandle(), nullptr, TRUE);
}

void LIR::UI::Control::SetRenderMode(RenderMode renderMode) {
	if (_renderMode == renderMode) return;

	_renderMode = renderMode;
	if (_renderMode == RenderMode::Native) _renderer.reset();
	if (IsValid()) InvalidateRect(GetHandle(), nullptr, TRUE);
}

std::unique_ptr<LIR::UI::IControlRenderer>& LIR::UI::Control::GetRenderer() {
	return _renderer;
}

LIR::UI::RenderMode LIR::UI::Control::GetRenderMode() const {
	return _renderMode;
}