# LIRUI
## Example
```cpp
#include "UIWindow.hpp"
#include "UILabel.hpp"
#include "UIButton.hpp"
#include "UITextBox.hpp"
#include "UICheckBox.hpp"
#include "UIRadioButton.hpp"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	const int scrWidth = GetSystemMetrics(SM_CXSCREEN);
	const int scrHeight = GetSystemMetrics(SM_CYSCREEN);
	const int scrCenterX = scrWidth / 2;
	const int scrCenterY = scrHeight / 2;

	const int mainWindowWidth = scrWidth / 2;
	const int mainWindowHeight = scrHeight / 2;
	const int mainWindowX = scrCenterX - mainWindowWidth / 2;
	const int mainWindowY = scrCenterY - mainWindowHeight / 2;
	LIR::UI::Window mainWindow(nullptr, L"Test", mainWindowX, mainWindowY, mainWindowWidth, mainWindowHeight);
	mainWindow.Work();
	mainWindow.OnClose += [](LIR::UI::EventArgs& args) -> LIR::UI::EventResult {
		LIR::UI::EventResult result{};
		result.AllowDefault = false;			// prevent DefWindowProc/DefSubclassProc
		return result;
	};

	const int lblHeight = 80;
	LIR::UI::Label lbl(&mainWindow, L"Label", 0, 0, mainWindowWidth, lblHeight);
	lbl.Work();

	LIR::UI::Button btn(&mainWindow, L"Button", 0, lblHeight, mainWindowWidth, lblHeight);
	btn.Work();
	btn.OnClick += [](LIR::UI::ClickEventArgs& args) -> LIR::UI::EventResult {
		LIR::UI::CheckBox* checkBox = (LIR::UI::CheckBox*)args.Sender->GetParent()->GetChild(3);
		if (!checkBox) MessageBoxA(0, "No child... :(", "Error", MB_OK | MB_ICONERROR);
		else checkBox->SetTitleSync(checkBox->IsChecked() ? L"Checked" : L"Not checked");

		return LIR::UI::EventResult{};
	};

	const int txtBoxY = lblHeight * 2;
	LIR::UI::TextBox txtBox(&mainWindow, L"TextBox", 0, txtBoxY, mainWindowWidth, lblHeight);
	txtBox.Work();
	txtBox.OnTextChange += [](LIR::UI::TextChangeEventArgs& args) -> LIR::UI::EventResult {
		LIR::UI::TextBox* txtBox = (LIR::UI::TextBox*)args.Sender;
		args.Sender->GetParent()->SetTitleSync(txtBox->GetText());
		return LIR::UI::EventResult{};
	};

	const int checkBoxY = txtBoxY + lblHeight;
	LIR::UI::CheckBox checkBox(&mainWindow, L"CheckBox", 0, checkBoxY, mainWindowWidth, lblHeight);
	checkBox.Work();

	const int radioBtn1Y = checkBoxY + lblHeight;
	LIR::UI::RadioGroup radioGroup{};
	LIR::UI::RadioButton radioBtn1(&mainWindow, L"Radio Button 1", 0, radioBtn1Y, mainWindowWidth, lblHeight, &radioGroup);
	radioBtn1.Work();

	const int radioBtn2Y = radioBtn1Y + lblHeight;
	LIR::UI::RadioButton radioBtn2(&mainWindow, L"Radio Button 2", 0, radioBtn2Y, mainWindowWidth, lblHeight, &radioGroup);
	radioBtn2.Work();

	Sleep(20000);
	LIR::UI::Dispatcher::Shutdown();
	return 0;
}
```