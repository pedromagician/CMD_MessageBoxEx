#include "stdafx.h"

#ifdef _DEBUG
Debug theDebug;
#endif

#include "CommandLine.h"
#include "Conversion.h"
#include "MessageBoxEx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_setmode(_fileno(stdout), _O_U8TEXT);

	int correctParameters = 0;
	bool help = false;
	wstring monitor = _T("primary");

	map<wstring, int> string2positionType{
		{_T("center"),	(int)MessageBoxEx::_CENTER},
		{_T("top"),		(int)MessageBoxEx::_TOP_CENTER},
		{_T("bottom"),	(int)MessageBoxEx::_BOTTOM_CENTER},
		{_T("left"),	(int)MessageBoxEx::_LEFT_CENTER},
		{_T("right"),	(int)MessageBoxEx::_RIGHT_CENTER},
		{_T("pointer"),	(int)MessageBoxEx::_POINTER},
		{_T("xy"),		(int)MessageBoxEx::_XY},
	};
	int posType = (int)MessageBoxEx::_CENTER;

	map<wstring, int> string2buttons{
		{_T("0"),	0},
		{_T("1"),	1},
		{_T("2"),	2},
		{_T("3"),	3}
	};

	map<wstring, int> defString2buttons{
		{_T("1"),	1},
		{_T("2"),	2},
		{_T("3"),	3}
	};

	MessageBoxEx::TimeUntilEndOfApplication()		= -1;
	MessageBoxEx::MinimumDisplayTime()				= -1;
	MessageBoxEx::FileRequiredForCompletion()		= _T("");
	MessageBoxEx::DeleteFileRequiredForCompletion() = false;

	MessageBoxEx::Position().monitor	= MessageBoxEx::_PRIMARY;
	MessageBoxEx::Position().id			= 0;
	MessageBoxEx::Position().type		= MessageBoxEx::_CENTER;
	MessageBoxEx::Position().delta		= { 0, 0 };

	CommandLine cmd;
	{
		cmd.AddString({ _T("title"),	_T("t") },
			_T("Specifies the title text displayed in the dialog window."),
			MessageBoxEx::Title());

		cmd.AddBool({ _T("noTitle"),	_T("nt") },
			_T("Removes the title bar and border from the dialog, creating a borderless window."),
			MessageBoxEx::NoTitle());

		cmd.AddString({ _T("message"),	_T("m") },
			_T("Specifies the message text displayed in the dialog. Use \\n for line breaks; Unicode supported via \\u{XXXX}."),
			MessageBoxEx::Prompt());

		cmd.AddBool({ _T("help"),			_T("h"),	_T("?") },
			_T("Displays this help information."),
			help);

		cmd.AddString({ _T("font"),			_T("f") },
			_T("Specifies the font name used for rendering text in the dialog."),
			MessageBoxEx::FontName());

		cmd.AddInt({ _T("fontsize"),	_T("fs") },
			_T("Specifies the font size in points."),
			MessageBoxEx::FontSize());

		cmd.AddInt({ _T("lines"),	_T("l") },
			_T("Specifies the number of lines allocated for the message text area."),
			MessageBoxEx::LinesOfText());

		cmd.AddInt({ _T("width"),	_T("w") },
			_T("Specifies the width of the dialog window in pixels."),
			MessageBoxEx::Width());

		cmd.AddColor({ _T("pen"),			_T("p") },
			_T("Specifies the pen color (text foreground and gadget outline)."),
			MessageBoxEx::Pen().second);

		cmd.AddColor({ _T("background"),	_T("b") },
			_T("Specifies the background fill color of the dialog."),
			MessageBoxEx::Background().second);

		cmd.AddColor({ _T("brush"),	_T("br") },
			_T("Specifies the brush color (pattern or area fill)."),
			MessageBoxEx::Brush().second);

		cmd.AddString({ _T("icon"),			_T("i") },
			_T("Specifies the path to the image file displayed as an icon (BMP format)."),
			MessageBoxEx::IconFile());

		cmd.AddInt({ _T("iconsize") },
			_T("Specifies the width and height of the icon image in pixels."),
			MessageBoxEx::IconSize());

		cmd.AddBool({ _T("iconborder") },
			_T("Draws a visible border frame around the custom icon image."),
			MessageBoxEx::IconBorder());

		cmd.AddString({ _T("iconapp") },
			_T("Specifies the application icon file (ICO or BMP) shown in the window title bar or taskbar."),
			MessageBoxEx::IconApp());

		cmd.AddEnum({ _T("button") },
			_T("Specifies the number of buttons to display. Allowed options: 0|1|2|3."),
			string2buttons, MessageBoxEx::Buttons());

		cmd.AddInt({ _T("widthbuttons") },
			_T("Specifies the additional width (in pixels) added to each button beyond the default size."),
			MessageBoxEx::ButtonsWidth());

		cmd.AddBool({ _T("center") },
			_T("Centers the action buttons horizontally within the dialog."),
			MessageBoxEx::Center());

		cmd.AddString({ _T("b1") },
			_T("Specifies the label text for button 1."),
			MessageBoxEx::Button1());

		cmd.AddString({ _T("b2") },
			_T("Specifies the label text for button 2."),
			MessageBoxEx::Button2());

		cmd.AddString({ _T("b3") },
			_T("Specifies the label text for button 3."),
			MessageBoxEx::Button3());

		cmd.AddEnum({ _T("default"),	_T("d") },
			_T("Specifies which button is selected by default when the dialog opens. Allowed options: 1|2|3."),
			defString2buttons, MessageBoxEx::DefaultButton());

		cmd.AddString({ _T("monitor"),	_T("mon") },
			_T("Specifies the target monitor for display: Primary|Mouse|MousePointer|0|1|2|n (where n is monitor index)."),
			monitor);

		cmd.AddEnum({ _T("position"),	_T("pos") },
			_T("Specifies the initial dialog placement: Center|Top|Bottom|Left|Right|Pointer|xy (explicit coordinates)."),
			string2positionType, posType);

		cmd.AddInt({ _T("x") },
			_T("Specifies the X-axis position offset in pixels relative to the base position."),
			MessageBoxEx::Position().delta.x);

		cmd.AddInt({ _T("y") },
			_T("Specifies the Y-axis position offset in pixels relative to the base position."),
			MessageBoxEx::Position().delta.y);

		cmd.AddBool({ _T("topmost") },
			_T("Forces the dialog to remain above all non-topmost windows, even when deactivated."),
			MessageBoxEx::TopMost());

		cmd.AddBool({ _T("block") },
			_T("Blocks execution of the parent process until the dialog is closed (modal behavior)."),
			MessageBoxEx::BlockParent());

		cmd.AddBool({ _T("windowsReturnCode"),	_T("wrc") },
			_T("Enables Windows-style return codes instead of custom button IDs."),
			MessageBoxEx::WindowsReturnCode());

		cmd.AddBool({ _T("quiet") },
			_T("Suppresses output logging to the command line interface."),
			MessageBoxEx::Quiet());

		cmd.AddInt({ _T("minimumDisplayTime"),	_T("mdt") },
			_T("Specifies the minimum duration (in milliseconds) the dialog must remain visible before closing."),
			MessageBoxEx::MinimumDisplayTime());

		cmd.AddInt({ _T("timeToCompletion"),			_T("ttc") },
			_T("Specifies the timeout duration (in milliseconds) after which the dialog closes automatically."),
			MessageBoxEx::TimeUntilEndOfApplication());

		cmd.AddString({ _T("fileRequiredForCompletion"),	_T("frfc"),	_T("file") },
			_T("Specifies a file path; if this file exists, the dialog will close immediately."),
			MessageBoxEx::FileRequiredForCompletion());

		cmd.AddBool({ _T("deleteFileRequiredForCompletion"),	_T("dfrfc"),	_T("deleteFile") },
			_T("Automatically deletes the trigger file specified in 'file' after the dialog closes."),
			MessageBoxEx::DeleteFileRequiredForCompletion());
	}

	if (!cmd.ParseCommandLine(argc, argv, correctParameters) || correctParameters == 0 || help) {
		cmd.Help();
		return 0;
	}

	MessageBoxEx::Position().type = (MessageBoxEx::POSITION)posType;
	MessageBoxEx::Pen().first = cmd.WasProvided(L"pen");
	MessageBoxEx::Background().first = cmd.WasProvided(L"background");
	MessageBoxEx::Brush().first = cmd.WasProvided(L"brush");

	if (MessageBoxEx::Prompt().empty()) {
		wcout << _T("Error - message is empty") << endl;
		return 0;
	}

	monitor = Conversion::ToLower(Conversion::TrimWhiteChar(monitor));
	if (monitor == _T("primary"))			MessageBoxEx::Position().monitor = MessageBoxEx::_PRIMARY;
	else if (monitor == _T("mouse"))		MessageBoxEx::Position().monitor = MessageBoxEx::_MOUSE;
	else if (monitor == _T("mousepointer"))	MessageBoxEx::Position().monitor = MessageBoxEx::_MOUSE_POINTER;
	else {
		MessageBoxEx::Position().monitor = MessageBoxEx::_ID;
		MessageBoxEx::Position().id = (UINT)Conversion::ToInt(monitor);
	}

	int result = 0;
	if (MessageBoxEx::MessageBox(result)) {
		if (MessageBoxEx::WindowsReturnCode()) {
			if (result == 1) result = IDYES;
			else if (result == 2) result = IDNO;
			else if (result == 3) result = IDCANCEL;
		}

		if (MessageBoxEx::Quiet() == false)
			wcout << to_wstring(result);
	}

	return result;
}
