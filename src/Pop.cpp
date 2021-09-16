
#include "Pop.h"
#include "../res/resource.h"

Pop::Pop() : ModalDialog{DLG_MAIN}
{
}

INT_PTR Pop::dialogProc(UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_COMMAND:
		switch LOWORD(wp) {
		case IDCANCEL: SendMessage(hWnd(), WM_CLOSE, 0, 0); return TRUE;
		}
		return FALSE;
	case WM_CLOSE: EndDialog(hWnd(), 0); return TRUE;
	}
	return FALSE;
}
