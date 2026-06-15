#include "stdafx.h"
#include "MessageBoxEx.h"
#include "Conversion.h"
#include "Monitors.h"

static HFONT	mhFont		= nullptr;
static HWND		mhWndParent	= nullptr;
static HWND		mhWndPrompt	= nullptr;
static HWND		mhWndMessageBoxEx	= nullptr;
static HWND		mhWnd1		= nullptr;
static HWND		mhWnd2		= nullptr;
static HWND		mhWnd3		= nullptr;
static HWND		mhWndIcon	= nullptr;
static HBRUSH	mhbrBkgnd	= nullptr;
static int		mResultFromButtons	= 0;
static bool		mRunning	= true;
static HBITMAP	mIcon		= nullptr;
static HICON    mAppIcon	= nullptr;


void MessageBoxEx::SetTextAlignment(HWND _hwnd, int _textAlignment)
{
	LONG_PTR style = GetWindowLongPtr(_hwnd, GWL_STYLE);
	LONG_PTR currentAlign = style & (SS_LEFT | SS_CENTER | SS_RIGHT);

	LONG_PTR desiredAlign = 0;
	if (_textAlignment == 0)      desiredAlign = SS_LEFT;
	else if (_textAlignment == 1) desiredAlign = SS_CENTER;
	else if (_textAlignment == 2) desiredAlign = SS_RIGHT;

	if (currentAlign == desiredAlign)
		return;

	style &= ~(SS_LEFT | SS_CENTER | SS_RIGHT);

	style |= desiredAlign;

	if (SetWindowLongPtr(_hwnd, GWL_STYLE, style) == 0) {
		if (GetLastError() != 0) {
			wcout << L"Error - SetWindowLongPtr failed for alignment" << endl;
		}
	}

	if (!SetWindowPos(_hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_DRAWFRAME)) {
		wcout << L"Error - SetWindowPos failed" << endl;
	}
}

LRESULT CALLBACK MessageBoxEx::WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	LOGFONT lfont;
	HINSTANCE hInst = nullptr;

	switch (_message) {
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		{
			int r = 0, g = 0, b = 0;
			if (Brush().first) {
				if (mhbrBkgnd == nullptr) {
					r = Brush().second.r;
					g = Brush().second.g;
					b = Brush().second.b;
					mhbrBkgnd = CreateSolidBrush(RGB(r, g, b));
				}
			}

			HDC hdcStatic = (HDC)_wParam;
			if (Pen().first) {
				r = Pen().second.r;
				g = Pen().second.g;
				b = Pen().second.b;
				SetTextColor(hdcStatic, RGB(r, g, b));
			}

			if (Background().first) {
				r = Background().second.r;
				g = Background().second.g;
				b = Background().second.b;
				SetBkColor(hdcStatic, RGB(r, g, b));
			}

			return (INT_PTR)mhbrBkgnd;
			break;
		}
		case WM_CREATE: {
			memset(&lfont, 0, sizeof(lfont));
			lstrcpy(lfont.lfFaceName, (LPWSTR)MessageBoxEx::FontName().c_str());
			lfont.lfHeight			= FontSize();
			lfont.lfWeight			= FW_NORMAL;//FW_BOLD;
			lfont.lfItalic			= FALSE;
			lfont.lfCharSet			= DEFAULT_CHARSET;
			lfont.lfOutPrecision	= OUT_DEFAULT_PRECIS;
			lfont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
			lfont.lfQuality			= DEFAULT_QUALITY;
			lfont.lfPitchAndFamily	= DEFAULT_PITCH;
			mhFont = CreateFontIndirect(&lfont);

			hInst = GetModuleHandle(nullptr);

			// buttons
			if (MessageBoxEx::Buttons() > 0) {
				int buttonX = 0;
				int buttonWidth = FontSize() * 120 / 22 + MessageBoxEx::ButtonsWidth();
				int buttonHeight = FontSize() + 8;
				int buttonY = 10 + FontSize() / 2 + FontSize() * LinesOfText() + 10;

				// button 1
				if (MessageBoxEx::Center()) {
					if (MessageBoxEx::Buttons() == 1)
						buttonX = MessageBoxEx::Width() / 2 - buttonWidth / 2;
					else if (MessageBoxEx::Buttons() == 2)
						buttonX = MessageBoxEx::Width() / 2 - (buttonWidth * 2 + 20) / 2 - 10;
					else
						buttonX = MessageBoxEx::Width() / 2 - (buttonWidth * 3 + 40) / 2 - 5;
				}
				else {
					if (MessageBoxEx::Buttons() == 1)
						buttonX = MessageBoxEx::Width() - 25 - buttonWidth * 1 - 10;
					else if (MessageBoxEx::Buttons() == 2)
						buttonX = MessageBoxEx::Width() - 25 - buttonWidth * 2 - 10 - 20;
					else
						buttonX = MessageBoxEx::Width() - 25 - buttonWidth * 3 - 10 - 20 * 2;
				}

				mhWnd1 = CreateWindowEx(WS_EX_STATICEDGE, _T("Button"), MessageBoxEx::Button1().c_str(), WS_VISIBLE | WS_CHILD | WS_TABSTOP, buttonX, buttonY, buttonWidth, buttonHeight, _hWnd, nullptr, hInst, nullptr);
				if (mhWnd1 == nullptr) return (LRESULT)nullptr;
				SendMessage((mhWnd1), WM_SETFONT, (WPARAM)mhFont, 0);

				// button 2
				if (MessageBoxEx::Buttons() >= 2) {
					if (MessageBoxEx::Center()) {
						if (MessageBoxEx::Buttons() == 2)
							buttonX = MessageBoxEx::Width() / 2 - (buttonWidth * 2 + 20) / 2 + buttonWidth + 20 - 10;
						else
							buttonX = MessageBoxEx::Width() / 2 - (buttonWidth * 3 + 40) / 2 + (buttonWidth + 20) * 1 - 5;
					}
					else {
						if (MessageBoxEx::Buttons() == 2)
							buttonX = MessageBoxEx::Width() - 25 - buttonWidth * 1 - 10;
						else
							buttonX = MessageBoxEx::Width() - 25 - buttonWidth * 2 - 10 - 20;
					}

					mhWnd2 = CreateWindowEx(WS_EX_STATICEDGE, _T("Button"), MessageBoxEx::Button2().c_str(), WS_VISIBLE | WS_CHILD | WS_TABSTOP, buttonX, buttonY, buttonWidth, buttonHeight, _hWnd, nullptr, hInst, nullptr);
					if (mhWnd2 == nullptr) return (LRESULT)nullptr;
					SendMessage((mhWnd2), WM_SETFONT, (WPARAM)mhFont, 0);
				}

				// button 3
				if (MessageBoxEx::Buttons() == 3) {
					if (MessageBoxEx::Center())
						buttonX = MessageBoxEx::Width() / 2 - (buttonWidth * 3 + 40) / 2 + (buttonWidth + 20) * 2 - 5;
					else
						buttonX = MessageBoxEx::Width() - 25 - buttonWidth * 1 - 10;

					mhWnd3 = CreateWindowEx(WS_EX_STATICEDGE, _T("Button"), MessageBoxEx::Button3().c_str(), WS_VISIBLE | WS_CHILD | WS_TABSTOP, buttonX, buttonY, buttonWidth, buttonHeight, _hWnd, nullptr, hInst, nullptr);
					if (mhWnd3 == nullptr) return (LRESULT)nullptr;
					SendMessage((mhWnd3), WM_SETFONT, (WPARAM)mhFont, 0);
				}
			}

			// icon
			if (MessageBoxEx::IconFile().empty() == false) {
				if (MessageBoxEx::IconSize() == 0)
					MessageBoxEx::IconSize() = 48;

				if (mIcon) {
					DeleteObject(mIcon);
					mIcon = nullptr;
				}
				mIcon = (HBITMAP)LoadImage(hInst, MessageBoxEx::IconFile().c_str(), IMAGE_BITMAP, MessageBoxEx::IconSize(), MessageBoxEx::IconSize(), LR_LOADFROMFILE);

				if (mIcon) {
					if (MessageBoxEx::IconBorder())	mhWndIcon = CreateWindow(_T("Static"), NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | WS_BORDER	, 10, 10, MessageBoxEx::IconSize(), MessageBoxEx::IconSize(), _hWnd, NULL, hInst, NULL);
					else							mhWndIcon = CreateWindow(_T("Static"), NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP 				, 10, 10, MessageBoxEx::IconSize(), MessageBoxEx::IconSize(), _hWnd, NULL, hInst, NULL);
					SendMessage(mhWndIcon, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)mIcon);
				}
				else {
					//MessageBoxEx::IconSize() = 0;
					//MessageBoxEx::IconBorder() = false;
					mhWndIcon = CreateWindow(_T("Static"), NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | WS_BORDER, 10, 10, MessageBoxEx::IconSize(), MessageBoxEx::IconSize(), _hWnd, NULL, hInst, NULL);
				}
			}
			else {
				MessageBoxEx::IconSize() = 0;
				MessageBoxEx::IconBorder() = false;
			}

			// icon app
			if (MessageBoxEx::IconApp().empty() == false) {
				HICON hIcon = (HICON)LoadImage(hInst, MessageBoxEx::IconApp().c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
				if (!hIcon) {
					HBITMAP bitmapForIconApp = (HBITMAP)LoadImage(hInst, MessageBoxEx::IconApp().c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
					if (bitmapForIconApp) {
						ICONINFO ii = { 0 };
						ii.fIcon = TRUE;
						ii.hbmColor = bitmapForIconApp;
						ii.hbmMask = bitmapForIconApp;

						hIcon = ::CreateIconIndirect(&ii);

						::DeleteObject(bitmapForIconApp);
					}
				}

				if (hIcon) {
					mAppIcon = hIcon;
					SendMessage(_hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);
					SendMessage(_hWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIcon);
				}
			}

			// message
			int messX = 5;
			int messY = 10;
			int messWidth = MessageBoxEx::Width() - 30;
			int messHeight = FontSize() * LinesOfText();

			if (MessageBoxEx::IconSize() > 0) {
				messX = messX + MessageBoxEx::IconSize() + 15;
				messWidth = messWidth - MessageBoxEx::IconSize() - 15;
			}

			mhWndPrompt = CreateWindowEx(NULL, _T("Static"), _T(""), WS_VISIBLE | WS_CHILD, messX, messY, messWidth, messHeight, _hWnd, nullptr, hInst, nullptr);
			if (mhWndPrompt == nullptr) return (LRESULT)nullptr;
			SendMessage((mhWndPrompt), WM_SETFONT, (WPARAM)mhFont, 0);

			// position
			RECT dialogRect;
			GetWindowRect(_hWnd, &dialogRect);

			bool monitor = true;
			RECT monitorSize = { 0 };

			switch (MessageBoxEx::Position().monitor) {
			case _PRIMARY:
				monitor = Monitors::GetMonitorInfoPrimary(monitorSize);
				break;
			case _MOUSE:
				monitor = Monitors::GetMonitorInfoMouse(monitorSize);
				break;
			case _MOUSE_POINTER:
				monitor = Monitors::GetMonitorInfoMouse(monitorSize);
				MessageBoxEx::Position().type = _POINTER;
				break;
			case _ID:
				monitor = Monitors::GetMonitorInfoId(MessageBoxEx::Position().id, monitorSize);
				break;
			}

			if (MessageBoxEx::Position().type == _POINTER) {
				RECT mouseMonitorSize = { 0 };
				monitor = Monitors::GetMonitorInfoMouse(mouseMonitorSize);
				if (EqualRect(&mouseMonitorSize, &monitorSize) == false) {
					MessageBoxEx::Position().type = _CENTER;
				}
			}

			long x = 0;
			long y = 0;
			if (monitor) {
				switch (MessageBoxEx::Position().type) {
				case _CENTER: {
					x = GetDiameterX(monitorSize) - GetWidth(dialogRect) / 2;
					y = GetDiameterY(monitorSize) - GetHeight(dialogRect) / 2;
					break;
				}
				case _XY: {
					x = monitorSize.left;
					y = monitorSize.top;
					break;
				}
				case _TOP_CENTER: {
					x = GetDiameterX(monitorSize) - GetWidth(dialogRect) / 2;
					y = monitorSize.top;
					break;
				}
				case _BOTTOM_CENTER: {
					x = GetDiameterX(monitorSize) - GetWidth(dialogRect) / 2;
					y = monitorSize.bottom - GetHeight(dialogRect);
					break;
				}
				case _LEFT_CENTER: {
					x = monitorSize.left;
					y = GetDiameterY(monitorSize) - GetHeight(dialogRect) / 2;
					break;
				}
				case _RIGHT_CENTER: {
					x = monitorSize.right - GetWidth(dialogRect);
					y = GetDiameterY(monitorSize) - GetHeight(dialogRect) / 2;
					break;
				}
				case _POINTER: {
					POINT mouse;
					::GetCursorPos(&mouse);

					x = mouse.x - GetWidth(dialogRect) / 2;
					y = mouse.y - GetHeight(dialogRect) / 2;

					break;
				}
				default:
					wcout << _T("Error - unknown position: ") + to_wstring(MessageBoxEx::Position().type) << endl;
					MessageBoxEx::Position().type = _CENTER;
					monitor = Monitors::GetMonitorInfoPrimary(monitorSize);
					x = GetDiameterX(monitorSize) - GetWidth(dialogRect) / 2;
					y = GetDiameterY(monitorSize) - GetHeight(dialogRect) / 2;
					break;
				}
			}
			else {
				wcout << _T("Error - problem loading information from the monitor") << endl;
				MessageBoxEx::Position().monitor = _PRIMARY;
				MessageBoxEx::Position().type = _XY;
				x = monitorSize.left;
				y = monitorSize.top;
			}

			x += MessageBoxEx::Position().delta.x;
			y += MessageBoxEx::Position().delta.y;

			UINT flags = SWP_NOSIZE | SWP_SHOWWINDOW;
			if (MessageBoxEx::TopMost() == false)
				flags |= SWP_NOZORDER;

			if (!SetWindowPos(_hWnd, HWND_TOPMOST, x, y, 0, 0, flags)) {
				wcout << L"Error - SetWindowPos failed" << endl;
			}
			break;
		}
		case WM_DESTROY: {
			mRunning = false;

			if (mhFont) {
				DeleteObject(mhFont);
				mhFont = nullptr;
			}

			if (mhbrBkgnd) {
				DeleteObject(mhbrBkgnd);
				mhbrBkgnd = nullptr;
			}

			if (mAppIcon) {
				DestroyIcon(mAppIcon);
				mAppIcon = nullptr;
			}

			if (mIcon) {
				DeleteObject(mIcon);
				mIcon = nullptr;
			}

			DestroyWindow(_hWnd);
			PostQuitMessage(0);
			break;
		}
		case WM_COMMAND: {
			switch (HIWORD(_wParam)) {
				case BN_CLICKED:
					if ((HWND)_lParam == mhWnd1) {
						mResultFromButtons = 1;
						PostMessage(mhWndMessageBoxEx, WM_KEYDOWN, VK_RETURN, 0);
					}
					if ((HWND)_lParam == mhWnd2) {
						mResultFromButtons = 2;
						PostMessage(mhWndMessageBoxEx, WM_KEYDOWN, VK_RETURN, 0);
					}
					if ((HWND)_lParam == mhWnd3) {
						mResultFromButtons = 3;
						PostMessage(mhWndMessageBoxEx, WM_KEYDOWN, VK_RETURN, 0);
					}
					break;
			}
			break;
		}
		default: {
			return DefWindowProc(_hWnd, _message, _wParam, _lParam);
		}
	}
	return 0;
}

bool MessageBoxEx::MessageBox(int& _result)
{
	mRunning = true;
	mResultFromButtons = 0;

	Conversion::UnicodeCodeConverter(Title());
	Conversion::UnicodeCodeConverter(Prompt());
	Conversion::UnicodeCodeConverter(Button1());
	Conversion::UnicodeCodeConverter(Button2());
	Conversion::UnicodeCodeConverter(Button3());

	mhWndParent = GetActiveWindow();
	if (!mhWndParent)
		mhWndParent = GetForegroundWindow();
	if (!mhWndParent)
		mhWndParent = GetDesktopWindow();

	RECT rc;
	GetWindowRect(mhWndParent, &rc);

	HINSTANCE hInst = GetModuleHandle(nullptr);
	WNDCLASSEX wcex;
	if (!GetClassInfoEx(hInst, _T("MessageBoxEx"), &wcex)) {
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = (WNDPROC)WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = nullptr;
		wcex.hCursor = nullptr;
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = _T("MessageBoxEx");
		wcex.hIconSm = nullptr;

		if (RegisterClassEx(&wcex) == 0) {
			wcout << _T("Error - register class") << endl;
			return false;
		}
	}

	// window
	int buttonY = 10 + FontSize() / 2 + FontSize() * LinesOfText() + 10;
	int buttonHeight = FontSize() + 8;

	HWND parent = nullptr;
	if (MessageBoxEx::BlockParent())
		parent = mhWndParent;

	DWORD dwStyle = WS_POPUPWINDOW | WS_CAPTION | WS_TABSTOP | WS_VISIBLE;
	dwStyle = dwStyle & ~WS_MAXIMIZEBOX;
	dwStyle = dwStyle & ~WS_MINIMIZEBOX;

	int width = MessageBoxEx::Width();
	int height = 50 + buttonY + buttonHeight;
	if (MessageBoxEx::NoTitle()) {
		dwStyle = dwStyle & ~WS_CAPTION;
		width -= (GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXBORDER)) * 2;
		height -= (GetSystemMetrics(SM_CYCAPTION) + (GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYBORDER)) * 2);
	}
	if (width < 0) width = 0;
	if (height < 0) height = 0;

	mhWndMessageBoxEx = CreateWindowEx(
		WS_EX_DLGMODALFRAME, _T("MessageBoxEx"), MessageBoxEx::Title().c_str(), dwStyle,
		(rc.right - width) / 2, (rc.bottom - height) / 2,
		width, height,
		parent, nullptr, nullptr, nullptr
	);
	if (mhWndMessageBoxEx == nullptr)
		return false;

	HMENU pSysMenu = GetSystemMenu(mhWndMessageBoxEx, FALSE);
	if (pSysMenu) {
		RemoveMenu(pSysMenu, SC_RESTORE, MF_BYCOMMAND);
		RemoveMenu(pSysMenu, SC_SIZE, MF_BYCOMMAND);
		RemoveMenu(pSysMenu, SC_MINIMIZE, MF_BYCOMMAND);
		RemoveMenu(pSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
	}

	SetTextAlignment(mhWndPrompt, SS_LEFT);
	SetWindowText(mhWndPrompt, Prompt().c_str());
	SetForegroundWindow(mhWndMessageBoxEx);

	SendMessage((HWND)mhWnd1, BM_SETSTYLE, (WPARAM)LOWORD(BS_PUSHBUTTON), MAKELPARAM(TRUE, 0));
	SendMessage((HWND)mhWnd2, BM_SETSTYLE, (WPARAM)LOWORD(BS_PUSHBUTTON), MAKELPARAM(TRUE, 0));
	SendMessage((HWND)mhWnd3, BM_SETSTYLE, (WPARAM)LOWORD(BS_PUSHBUTTON), MAKELPARAM(TRUE, 0));
	
	// default button
	if (MessageBoxEx::Buttons() > 0) {
		if (MessageBoxEx::DefaultButton() < 0 || MessageBoxEx::DefaultButton() > MessageBoxEx::Buttons())
			MessageBoxEx::DefaultButton() = 1;

		if (MessageBoxEx::DefaultButton() == 1)		SetFocus(mhWnd1);
		else if (MessageBoxEx::DefaultButton() == 2)	SetFocus(mhWnd2);
		else if (MessageBoxEx::DefaultButton() == 3)	SetFocus(mhWnd3);
	}

	if (MessageBoxEx::BlockParent())
		EnableWindow(mhWndParent, FALSE);
	ShowWindow(mhWndMessageBoxEx, SW_SHOW);
	UpdateWindow(mhWndMessageBoxEx);

	if (MessageBoxEx::Buttons() == 0)
		SetFocus(mhWndMessageBoxEx);

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	DWORD startTick = GetTickCount();

	if (MessageBoxEx::MinimumDisplayTime() > 0)
		Sleep(MessageBoxEx::MinimumDisplayTime());

	MSG msg;
	while (mRunning) {
		if (
			(MessageBoxEx::TimeUntilEndOfApplication() > 0 && GetTickCount() > (startTick + MessageBoxEx::TimeUntilEndOfApplication())) ||
			(MessageBoxEx::FileRequiredForCompletion().empty() == false && PathFileExistsW(MessageBoxEx::FileRequiredForCompletion().c_str()))
		) {
			mResultFromButtons = MessageBoxEx::DefaultButton();
			HWND hWndFocused = GetFocus();
			if (hWndFocused) {
				if (hWndFocused == mhWnd1)
					mResultFromButtons = 1;
				else if (hWndFocused == mhWnd2)
					mResultFromButtons = 2;
				else if (hWndFocused == mhWnd3)
					mResultFromButtons = 3;
			}
			mRunning = false;
			break;
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
			if (msg.message == WM_KEYDOWN) {
				if (msg.wParam == VK_ESCAPE) {
					mResultFromButtons = 0;
					SendMessage(mhWndMessageBoxEx, WM_DESTROY, 0, 0);
					mRunning = false;
					break;
				} else if (msg.wParam == VK_RETURN) {
					mResultFromButtons = MessageBoxEx::DefaultButton();
					HWND hWndFocused = GetFocus();
					if (hWndFocused) {
						if (hWndFocused == mhWnd1)
							mResultFromButtons = 1;
						else if (hWndFocused == mhWnd2)
							mResultFromButtons = 2;
						else if (hWndFocused == mhWnd3)
							mResultFromButtons = 3;
					}
					SendMessage(mhWndMessageBoxEx, WM_DESTROY, 0, 0);
					mRunning = false;
					break;
				} else if (msg.wParam == VK_TAB) {
					HWND hWndFocused = GetFocus();
					HWND tmpFocus = mhWnd1;

					if (MessageBoxEx::Buttons() > 1) {
						if (MessageBoxEx::Buttons() == 2) {
							if (hWndFocused == mhWnd1)
								tmpFocus = mhWnd2;
						}
						else {
							bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
							if (!shift)
							{
								if (hWndFocused == mhWnd1) tmpFocus = mhWnd2;
								else if (hWndFocused == mhWnd2) tmpFocus = mhWnd3;
								else tmpFocus = mhWnd1;
							}
							else
							{
								if (hWndFocused == mhWnd3) tmpFocus = mhWnd2;
								else if (hWndFocused == mhWnd2) tmpFocus = mhWnd1;
								else tmpFocus = mhWnd3;
							}
						}
					}

					SetFocus(tmpFocus);
				}
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	_result = mResultFromButtons;

	if (MessageBoxEx::FileRequiredForCompletion().empty() == false && PathFileExistsW(MessageBoxEx::FileRequiredForCompletion().c_str()) && MessageBoxEx::DeleteFileRequiredForCompletion())
		DeleteFile(MessageBoxEx::FileRequiredForCompletion().c_str());

	EnableWindow(mhWndParent, TRUE);
	SetForegroundWindow(mhWndParent);
	return true;
}

long MessageBoxEx::GetDiameterX(RECT _rect)
{
	return _rect.left + (_rect.right - _rect.left) / 2;
}

long MessageBoxEx::GetDiameterY(RECT _rect)
{
	return _rect.top + (_rect.bottom - _rect.top) / 2;
}

long MessageBoxEx::GetWidth(RECT _rect)
{
	return (_rect.right - _rect.left);
}

long MessageBoxEx::GetHeight(RECT _rect)
{
	return (_rect.bottom - _rect.top);
}
