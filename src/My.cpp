
#include "My.h"
#include "../res/resource.h"

RUN(My)

My::My()
{
	wnd.setup.handler = this;
	wnd.setup.dialogId = DLG_MAIN;
	wnd.setup.iconId = ICO_DUCREUX;
}

INT_PTR My::dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG: this->onInitDialog(hDlg); return TRUE;
	case WM_COMMAND:
		switch LOWORD(wp) {
		case IDCANCEL: SendMessage(hDlg, WM_CLOSE, 0, 0); return TRUE;
		}
		return FALSE;
	case WM_CLOSE: DestroyWindow(hDlg); return TRUE;
	case WM_NCDESTROY: PostQuitMessage(0); return TRUE;
	}
	return FALSE;
}

void My::onInitDialog(HWND hDlg)
{

}
