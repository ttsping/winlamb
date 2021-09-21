
#include <string>
#include <system_error>
#include "MainDialog.h"
#include "Icon.h"
#include "str.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")
using namespace core;
using std::optional;
using std::system_error;
using std::wstring;

int MainDialog::run(HINSTANCE hInst, int cmdShow)
{
	InitCommonControls();

	HWND hDlg = CreateDialogParamW(hInst, MAKEINTRESOURCEW(this->dialogId),
		nullptr, Dialog::Proc, (LPARAM)this); // pass obj pointer to proc
	if (!hDlg) {
		throw system_error(GetLastError(), std::system_category(), "CreateDialogParamW failed");
	}
	this->putWindowIcon(hDlg);
	ShowWindow(hDlg, cmdShow);

	HACCEL hAccel = this->accelId
		? LoadAcceleratorsW(hInst, MAKEINTRESOURCEW(this->accelId))
		: nullptr;
	return this->loop(hDlg, hAccel);
}

void MainDialog::putWindowIcon(HWND hDlg)
{
	if (!this->iconId) return;
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
		
	Icon ico16;
	ico16.loadResource(this->iconId, SIZE{16, 16}, optional{hInst});
	SendMessageW(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)ico16.hIcon());

	Icon ico32;
	ico32.loadResource(this->iconId, SIZE{32, 32}, optional{hInst});
	SendMessageW(hDlg, WM_SETICON, ICON_BIG, (LPARAM)ico32.hIcon());
}

int MainDialog::loop(HWND hDlg, HACCEL hAccel)
{
	MSG msg;

	for (;;) {
		if (BOOL ret = GetMessageW(&msg, nullptr, 0, 0); ret == -1) {
			throw system_error(GetLastError(), std::system_category(), "GetMessageW failed");
		} else if (ret == 0) { // WM_QUIT was sent, exit gracefully
			break;
		}

		HWND hTopLevel = GetAncestor(hDlg, GA_ROOT);
		if (hAccel && !TranslateAcceleratorW(hTopLevel, hAccel, &msg)) {
			continue; // message translated, no further processing is done
		}

		if (IsDialogMessageW(hDlg, &msg)) {
			continue; // processed all keyboard actions for child controls
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}
