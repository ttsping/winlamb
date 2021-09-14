
#include <Windows.h>
#include "../core/MainDialog.h"
#include "../res/resource.h"

static core::MainDialog my;

void initDialog(HWND hDlg)
{

}

static INT_PTR CALLBACK dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG: initDialog(hDlg); return TRUE;
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

int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow)
{
	my.hInst = hInst;
	my.dlgId = DLG_MAIN;
	my.iconId = ICO_DUCREUX;
	my.cmdShow = cmdShow;
	my.dlgProc = dialogProc;

	return my.run();
}
