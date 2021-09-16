
#include "../core/str.h"
#include "My.h"
#include "Pop.h"
#include "../res/resource.h"

RUN(My)

My::My() : MainDialog{DLG_MAIN, ICO_DUCREUX, 0}
{
}

INT_PTR My::dialogProc(UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG: onInitDialog(); return TRUE;
	case WM_COMMAND:
		switch LOWORD(wp) {
		case IDCANCEL: SendMessage(hWnd(), WM_CLOSE, 0, 0); return TRUE;
		case IDOK: onOk(); return TRUE;
		}
		return FALSE;
	case WM_CLOSE: DestroyWindow(hWnd()); return TRUE;
	case WM_NCDESTROY: PostQuitMessage(0); return TRUE;
	}
	return FALSE;
}

void My::onInitDialog()
{
	SetWindowText(hWnd(), L"Main window here");
	lousa.create(this, 20, 20, 300, 200);
}

void My::onOk()
{
	Pop pop;
	pop.show(this);
}
