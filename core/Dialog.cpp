
#include <optional>
#include "Dialog.h"
#include "Font.h"
#include "internals.h"
using namespace core;
using std::optional;

INT_PTR CALLBACK Dialog::Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) noexcept
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

	optional<INT_PTR> maybeRet;

	if (pObj) {
		if (pObj->processUiThreadMsg(msg, wp, lp)) return TRUE;

		try {
			maybeRet = pObj->dialogProc(msg, wp, lp);
		} catch (...) {
			PostQuitMessage(core_internals::Lippincott());
		}

		if (msg == WM_NCDESTROY) {
			pObj->hw = nullptr;
			SetWindowLongPtrW(hDlg, DWLP_USER, 0);
		}
	}

	return maybeRet ? maybeRet.value() : FALSE;
}
