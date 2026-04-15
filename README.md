# LIRUI - lightweight wrapper for WinAPI UI

## Contents

- [UI Elements](#UIElements)
  - [BasicWindow](#BasicWindow)
  - [Window](#Window)
  - [Control](#Control)
  - [Label](#Label)
  - [Button](#Button)
  - [RadioButton](#RadioButton)
  - [CheckBox](#CheckBox)
  - [TextBox](#TextBox)
- [Use](#Use)
  <a id="UIElements"></a>

## UI Elements

| Parent                      | Element                     | Description                                                           |
| --------------------------- | --------------------------- | --------------------------------------------------------------------- |
| &mdash;                     | [BasicWindow](#BasicWindow) | The basic element on which the rest are based                         |
| [BasicWindow](#BasicWindow) | [Window](#Window)           | The window with a caption that serves as a container for all Controls |
| [BasicWindow](#BasicWindow) | [Control](#Control)         | The basic element for all Controls                                    |
| [Control](#Control)         | [Label](#Label)             | Regular label                                                         |
| [Control](#Control)         | [Button](#Button)           | Regular button                                                        |
| [Control](#Control)         | [RadioButton](#RadioButton) | Radio button that is associated with RadioGroup                       |
| [Control](#Control)         | [CheckBox](#CheckBox)       | Regular checkbox                                                      |
| [Control](#Control)         | [TextBox](#TextBox)         | Regular textbox                                                       |

<a id="BasicWindow"></a>

### BasicWindow

The basic element on which the rest are based.
<a id="BasicWindowMethods"></a>

#### Methods:

| Return type   | Name            | Parameters               | Description                                                                                                                            |
| ------------- | --------------- | ------------------------ | -------------------------------------------------------------------------------------------------------------------------------------- |
| bool          | IsValid         | &mdash;                  | If the element is currently running and it is valid - true                                                                             |
| BasicWindow\* | GetParent       | &mdash;                  | Get the parent element or nullptr if there is none                                                                                     |
| std::wstring  | GetTitle        | &mdash;                  | Get the title of an element (or its text)                                                                                              |
| bool          | GetPos          | int& x, int& y           | If successful, set the input parameters x,y to the position of the upper left corner of the window and returns true, otherwise - false |
| bool          | GetSize         | int& width, int& height  | If successful, set the input parameters width, height as the window size and returns true, otherwise - false                           |
| HWND          | GetHandle       | &mdash;                  | Get the WinAPI handle of the element (or nullptr)                                                                                      |
| std::wstring  | GetClass        | &mdash;                  | Get the WinAPI element class string                                                                                                    |
| DWORD         | GetStyles       | &mdash;                  | Get the WinAPI element styles                                                                                                          |
| DWORD         | GetExStyles     | &mdash;                  | Get extended WinAPI element styles                                                                                                     |
| bool          | SetTitleSync    | const std::wstring title | Set the element's title (or text)                                                                                                      |
| bool          | SetPosSync      | int x, int y             | Set the position of an element to its upper left corner                                                                                |
| bool          | SetSizeSync     | int width, int height    | Set the size of an element                                                                                                             |
| bool          | SetStylesSync   | DWORD styles             | Set WinAPI element styles                                                                                                              |
| bool          | SetExStylesSync | DWORD exStyles           | Set extended WinAPI element styles                                                                                                     |
| WorkResult    | Work            | &mdash;                  | Create an element as a WinAPI window and starts processing its events in the Dispatcher thread                                         |
| void          | Close           | &mdash;                  | Close the window and all its children (can be prevented inside a handler)                                                              |
| void          | Destroy         | &mdash;                  | Destroy the window and all its children (cannot be prevented)                                                                          |
| void          | AddChild        | BasicWindow\* window     | Add an element as child                                                                                                                |
| BasicWindow\* | GetChild        | size_t index             | Returns child by index                                                                                                                 |
| void          | RemoveChild     | size_t index             | Close an element by index                                                                                                              |
| void          | RemoveChild     | BasicWindow\* window     | Close an element by value                                                                                                              |
| size_t        | GetNumChildren  | &mdash;                  | Get the number of children                                                                                                             |
| WindowClass   | Class           | &mdash;                  | Get the window class as WindowClass                                                                                                    |

<a id="BasicWindowEventHandlers"></a>

#### Event handlers:

| Name         | Arguments            | Whence                                                          |
| ------------ | -------------------- | --------------------------------------------------------------- |
| OnCreate     | EventArgs&           | Element is created and its message queue begins to be processed |
| OnClose      | EventArgs&           | Element is "asked" to close (WM_CLOSE)                          |
| OnDestroy    | EventArgs&           | Element is forced closed (WM_DESTROY)                           |
| OnMouseEnter | MouseEnterEventArgs& | Mouse enters an element                                         |
| OnMouseLeave | MouseLeaveEventArgs& | Mouse leaves the element                                        |
| OnMouseMove  | MouseMoveEventArgs&  | Mouse moves within an element                                   |
| OnMouseDown  | MouseDownEventArgs&  | The mouse button was pressed on an element                      |
| OnMouseUp    | MouseUpEventArgs&    | The mouse button was released on an element                     |
| OnClick      | ClickEventArgs&      | The mouse clicked on an element                                 |
| OnDblClk     | DblClkEventArgs&     | The mouse clicked twice on an element                           |
| OnDragStart  | DragStartEventArgs&  | Started dragging with the mouse                                 |
| OnDrag       | DragEventArgs&       | Continue dragging with the mouse                                |
| OnDragEnd    | DragEndEventArgs&    | Ended up dragging with the mouse                                |
| OnFocus      | EventArgs&           | Focus was on this element                                       |
| OnFocusLose  | EventArgs&           | Focus was lost on an element                                    |
| OnKeyDown    | KeyDownEventArgs&    | A keyboard key was pressed on this element                      |
| OnKeyUp      | KeyUpEventArgs&      | A keyboard key was released on this element                     |
| OnInput      | InputEventArgs&      | An attempt was made to type a character                         |

<a id="Window"></a>

### Window

<a id="WindowMethods"></a>

#### Methods: All [BasicWindow](#BasicWindow) methods are inherited

<a id="WindowEventHandlers"></a>

#### Event handlers: All [BasicWindow](#BasicWindow) event handlers are inherited

<a id="Control"></a>

### Control

<a id="ControlMethods"></a>

#### Methods:

| Return type                        | Name          | Parameters                                 | Description                              |
| ---------------------------------- | ------------- | ------------------------------------------ | ---------------------------------------- |
| void                               | SetRenderer   | std::unique_ptr<IControlRenderer> renderer | Sets the renderer to display the element |
| void                               | SetRenderMode | RenderMode renderMode                      | Sets the display mode of an element      |
| std::unique_ptr<IControlRenderer>& | GetRenderer   | &mdash;                                    | Returns renderer                         |

All [BasicWindow](#BasicWindow) methods are inherited
<a id="ControlEventHandlers"></a>

#### Event handlers: All [BasicWindow](#BasicWindow) event handlers are inherited

<a id="Label"></a>

### Label

<a id="LabelMethods"></a>

#### Methods: All [Control](#Control) methods are inherited

<a id="LabelEventHandlers"></a>

#### Event handlers: All [Control](#Control) event handlers are inherited

<a id="Button"></a>

### Button

<a id="ButtonMethods"></a>

#### Methods: All [Control](#Control) methods are inherited

<a id="ButtonEventHandlers"></a>

#### Event handlers: All [Control](#Control) event handlers are inherited

<a id="RadioButton"></a>

### RadioButton

<a id="RadioButtonMethods"></a>

#### Methods:

| Return type | Name       | Parameters   | Description                   |
| ----------- | ---------- | ------------ | ----------------------------- |
| bool        | IsChecked  | &mdash;      | Get the state of that element |
| void        | SetChecked | bool checked | Set the state of that element |
| void        | Select     | &mdash;      | Select that element           |

All [Control](#Control) methods are inherited
<a id="RadioButtonEventHandlers"></a>

#### Event handlers: All [Control](#Control) event handlers are inherited

<a id="CheckBox"></a>

### CheckBox

<a id="CheckBoxMethods"></a>

#### Methods:

| Return type | Name        | Parameters   | Description                   |
| ----------- | ----------- | ------------ | ----------------------------- |
| bool        | IsChecked   | &mdash;      | Get the state of that element |
| void        | SetChecked  | bool checked | Set the state of that element |
| void        | ToggleState | &mdash;      | Select that element           |

All [Control](#Control) methods are inherited
<a id="CheckBoxEventHandlers"></a>

#### Event handlers: All [Control](#Control) event handlers are inherited

<a id="TextBox"></a>

### TextBox

<a id="TextBoxMethods"></a>

#### Methods:

| Return type  | Name    | Parameters              | Description      |
| ------------ | ------- | ----------------------- | ---------------- |
| std::wstring | GetText | &mdash;                 | Get element text |
| void         | SetText | const std::wstring text | Set element text |

All [Control](#Control) methods are inherited
<a id="TextBoxEventHandlers"></a>

#### Event handlers:

| Name         | Arguments            | Whence                |
| ------------ | -------------------- | --------------------- |
| OnTextChange | TextChangeEventArgs& | Text has been entered |

All [Control](#Control) event handlers are inherited

<a id="Use"></a>

## Use:

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
