
#include <optional>
#include <system_error>
#include "CustomControl.h"
#include "internals.h"
#include <vsstyle.h>
#include <Uxtheme.h>
#pragma comment(lib, "UxTheme.lib")
using namespace core;
using std::optional;
using std::system_error;

void CustomControl::create(Window* parent, int x, int y, int cx, int cy)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(parent->hWnd(), GWLP_HINSTANCE);

	WNDCLASSEX wcx = {0};
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.lpfnWndProc = CustomControl::Proc;
	wcx.hInstance = hInst;
	wcx.style = this->classStyles;
	wcx.hCursor = this->hCursor ? this->hCursor : LoadCursor(nullptr, IDC_ARROW);
	wcx.hbrBackground = this->hBrushBg ? this->hBrushBg : (HBRUSH)(COLOR_WINDOW + 1);

	// After all fields are set, we generate a class name by hashing all fields.
	wchar_t className[60] = {0};
	wsprintf(className, L"%Ix.%Ix.%Ix.%Ix.%Ix.%Ix.%Ix.%Ix.%Ix.%Ix",
		(UINT64)wcx.style, (UINT64)wcx.lpfnWndProc, (UINT64)wcx.cbClsExtra, (UINT64)wcx.cbWndExtra,
		(UINT64)wcx.hInstance, (UINT64)wcx.hIcon, (UINT64)wcx.hCursor, (UINT64)wcx.hbrBackground,
		(UINT64)wcx.lpszMenuName, (UINT64)wcx.hIconSm);

	wcx.lpszClassName = className;

	ATOM atom = RegisterClassEx(&wcx);
	if (!atom) {
		if (DWORD err = GetLastError(); err == ERROR_CLASS_ALREADY_EXISTS) {
			// https://devblogs.microsoft.com/oldnewthing/20150429-00/?p=44984
			// https://devblogs.microsoft.com/oldnewthing/20041011-00/?p=37603
			// Retrieve atom from existing window class.
			if (!(atom = GetClassInfoEx(hInst, className, &wcx))) {
				throw system_error(GetLastError(), std::system_category(), "GetClassInfoEx failed");
			}
		} else {
			throw system_error(err, std::system_category(), "RegisterClassExW failed");
		}
	}

	if (!CreateWindowEx(this->wndExStyles, MAKEINTATOM(atom), nullptr,
		this->wndStyles, x, y, cx, cy, parent->hWnd(), (HMENU)(UINT64)this->ctlId,
		hInst, (LPVOID)this)) // pass obj pointer to proc
	{
		throw system_error(GetLastError(), std::system_category(), "CreateWindowEx failed");
	}
}

WORD CustomControl::NextCtrlId()
{
	static WORD ctrlId = 20'000; // in-between Visual Studio Resource Editor values
	return ++ctrlId;
}

LRESULT CALLBACK CustomControl::Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CustomControl* pObj;

	if (msg == WM_NCCREATE) {
		CREATESTRUCT* cs = (CREATESTRUCT*)lp;
		pObj = (CustomControl*)cs->lpCreateParams;
		pObj->hw = hWnd;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pObj);
	} else {
		pObj = (CustomControl*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	if (CustomControl::PaintThemeBorders(hWnd, msg, wp, lp)) return 0;

	optional<LRESULT> maybeRet;

	if (pObj) {
		if (pObj->processUiThreadMsg(msg, wp, lp)) return 0;

		try {
			maybeRet = pObj->windowProc(msg, wp, lp);
		} catch (...) {
			PostQuitMessage(core_internals::Lippincott());
		}

		if (msg == WM_NCDESTROY) {
			pObj->hw = nullptr;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
		}
	}

	return maybeRet ? *maybeRet : DefWindowProc(hWnd, msg, wp, lp);
}

bool CustomControl::PaintThemeBorders(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg != WM_NCPAINT) return false;

	DefWindowProc(hWnd, WM_NCPAINT, wp, lp); // make system draw the scrollbar for us

	if (!(GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE) ||
		!IsThemeActive() ||
		!IsAppThemed()) return 0;

	RECT rc = {0};
	GetWindowRect(hWnd, &rc); // window outmost coordinates, including margins
	ScreenToClient(hWnd, (POINT*)&rc);
	ScreenToClient(hWnd, (POINT*)&rc.right);
	rc.left += 2; rc.top += 2; rc.right += 2; rc.bottom += 2; // because it comes up anchored at -2,-2

	RECT rcClip = {0}; // clipping region; will draw only within this rectangle
	HDC hdc = GetWindowDC(hWnd);
	if (HTHEME hTheme = OpenThemeData(hWnd, L"LISTVIEW"); hTheme) { // borrow style from listview
		SetRect(&rcClip, rc.left, rc.top, rc.left + 2, rc.bottom); // draw only the borders to avoid flickering
		DrawThemeBackground(hTheme, hdc, LVP_LISTGROUP, 0, &rc, &rcClip); // draw themed left border
		SetRect(&rcClip, rc.left, rc.top, rc.right, rc.top + 2);
		DrawThemeBackground(hTheme, hdc, LVP_LISTGROUP, 0, &rc, &rcClip); // draw themed top border
		SetRect(&rcClip, rc.right - 2, rc.top, rc.right, rc.bottom);
		DrawThemeBackground(hTheme, hdc, LVP_LISTGROUP, 0, &rc, &rcClip); // draw themed right border
		SetRect(&rcClip, rc.left, rc.bottom - 2, rc.right, rc.bottom);
		DrawThemeBackground(hTheme, hdc, LVP_LISTGROUP, 0, &rc, &rcClip); // draw themed bottom border

		CloseThemeData(hTheme);
	}
	ReleaseDC(hWnd, hdc);

	return true;
}
