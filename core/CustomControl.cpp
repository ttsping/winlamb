
#include "CustomControl.h"
#include "internals.h"
#include <vsstyle.h>
#include <Uxtheme.h>
#pragma comment(lib, "UxTheme.lib")
using namespace core;

DWORD CustomControl::create(Window* parent, int x, int y, int cx, int cy)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtrW(parent->hWnd(), GWLP_HINSTANCE);

	WNDCLASSEXW wcx = {0};
	wcx.cbSize = sizeof(WNDCLASSEXW);
	wcx.lpfnWndProc = CustomControl::Proc;
	wcx.hInstance = hInst;
	wcx.style = this->classStyles;
	wcx.hCursor = this->hCursor ? this->hCursor : LoadCursorW(nullptr, IDC_ARROW);
	wcx.hbrBackground = this->hBrushBg ? this->hBrushBg : (HBRUSH)(COLOR_WINDOW + 1);

	// After all fields are set, we generate a class name by hashing all fields.
	wchar_t className[60] = {0};
	wsprintfW(className, L"%Ix.%Ix.%Ix.%Ix.%Ix.%Ix.%Ix.%Ix.%Ix.%Ix",
		(UINT64)wcx.style, (UINT64)wcx.lpfnWndProc, (UINT64)wcx.cbClsExtra, (UINT64)wcx.cbWndExtra,
		(UINT64)wcx.hInstance, (UINT64)wcx.hIcon, (UINT64)wcx.hCursor, (UINT64)wcx.hbrBackground,
		(UINT64)wcx.lpszMenuName, (UINT64)wcx.hIconSm);

	wcx.lpszClassName = className;

	ATOM atom = RegisterClassExW(&wcx);
	if (!atom) {
		DWORD err = GetLastError();
		if (err == ERROR_CLASS_ALREADY_EXISTS) {
			// https://devblogs.microsoft.com/oldnewthing/20150429-00/?p=44984
			// https://devblogs.microsoft.com/oldnewthing/20041011-00/?p=37603
			// Retrieve atom from existing window class.
			if (!(atom = GetClassInfoExW(hInst, className, &wcx))) {
				return GetLastError();
			}
		} else {
			return err;
		}
	}

	if (!CreateWindowExW(this->wndExStyles, MAKEINTATOM(atom), nullptr,
		this->wndStyles, x, y, cx, cy, parent->hWnd(), (HMENU)(UINT64)this->ctlId,
		hInst, (LPVOID)this)) // pass obj pointer to proc
	{
		return GetLastError();
	}

	return ERROR_SUCCESS;
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
		CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
		pObj = (CustomControl*)cs->lpCreateParams;
		pObj->hw = hWnd;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pObj);
	} else {
		pObj = (CustomControl*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	}

	if (msg == WM_NCPAINT) { // will never be handled by the user
		DefWindowProcW(hWnd, WM_NCPAINT, wp, lp); // make system draw the scrollbar for us

		if (!(GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE) ||
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
		return 0;
	}

	if (pObj) {
		try {
			return pObj->windowProc(msg, wp, lp);
		} catch (...) {
			PostQuitMessage(core_internals::Lippincott());
		}
	}

	return DefWindowProcW(hWnd, msg, wp, lp);
}
