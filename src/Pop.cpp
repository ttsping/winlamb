
#include "Pop.h"
#include "../res/resource.h"

Pop::Pop() : ModalDialog{DLG_MAIN}
{
}

INT_PTR Pop::dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_COMMAND:
		switch LOWORD(wp) {
		case IDCANCEL: SendMessage(hDlg, WM_CLOSE, 0, 0); return TRUE;
		}
		return FALSE;
	case WM_CLOSE: EndDialog(hDlg, 0); return TRUE;
	}
	return FALSE;
}
