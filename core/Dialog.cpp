
#include "Dialog.h"
#include "Font.h"
using namespace core;

INT_PTR CALLBACK Dialog::Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	Dialog* pObj;

	if (msg == WM_INITDIALOG) {
		pObj = (Dialog*)lp;
		pObj->hw = hDlg;
		SetWindowLongPtrW(hDlg, DWLP_USER, (LONG_PTR)pObj);
		EnumChildWindows(hDlg, [](HWND hCtrl, LPARAM lp) -> BOOL {
			SendMessageW(hCtrl, WM_SETFONT, (WPARAM)(HFONT)lp, MAKELPARAM(FALSE, 0));
			return TRUE;
		}, (LPARAM)core::Font::UiFont().hFont());
	} else {
		pObj = (Dialog*)GetWindowLongPtrW(hDlg, DWLP_USER);
	}

	if (pObj) {
		try {
			return pObj->dialogProc(msg, wp, lp);
		} catch (...) {
			PostQuitMessage(Lippincott());
		}
	}

	return FALSE;
}
