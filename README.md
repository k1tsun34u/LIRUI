# LIRUI
## Example
```cpp
#include "UIWindow.hpp"
#include "UIButton.hpp"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	LIR::UI::Window mainWnd1(nullptr, L"A", 0, 0, 800, 600);
	mainWnd1.Work();
	mainWnd1.MouseMove += [](const LIR::UI::MouseMoveEventArgs& args) {
		std::wstring title = L"Mouse: X=";
		title += std::to_wstring(args.CurrentPos.x);
		title += L", Y=";
		title += std::to_wstring(args.CurrentPos.y);
		args.Sender->SetTitleSync(title);
	};

	LIR::UI::Button btn1(&mainWnd1, L"Click me!", 0, 0, 200, 80);
	btn1.Click += [](const LIR::UI::ClickEventArgs& args) {
		args.Sender->MouseMove += [](const LIR::UI::MouseMoveEventArgs& args) {
			int x, y;
			args.Sender->GetPos(x, y);

			args.Sender->SetPosSync(x + 1, y + 1);
		};
	};

	btn1.Work();
	LIR::UI::Button btn2(&btn1, L"Click me?", 10, 10, 50, 50);
	btn2.Click += [](const LIR::UI::ClickEventArgs& args) {
		args.Sender->GetParent()->Close();
	};
	btn2.Work();
	Sleep(20000);

	// Stop UI thread
	LIR::UI::Dispatcher::Shutdown();
	return 0;
}
```