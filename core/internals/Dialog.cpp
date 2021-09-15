
#include "../Font.h"
#include "Dialog.h"
using namespace core_internals;

INT_PTR Dialog::Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	Handler* pHandler;

	if (msg == WM_INITDIALOG) {
		pHandler = (Handler*)lp;
		SetWindowLongPtrW(hDlg, DWLP_USER, (LONG_PTR)pHandler);
		EnumChildWindows(hDlg, [](HWND hCtrl, LPARAM lp) -> BOOL {
			SendMessageW(hCtrl, WM_SETFONT, (WPARAM)(HFONT)lp, MAKELPARAM(FALSE, 0));
			return TRUE;
		}, (LPARAM)core::Font::UiFont().handle());
	} else {
		pHandler = (Handler*)GetWindowLongPtrW(hDlg, DWLP_USER);
	}

	return pHandler ? pHandler->dialogProc(hDlg, msg, wp, lp) : FALSE;
}
