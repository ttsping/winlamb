
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

	HWND hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(this->dialogId),
		nullptr, Dialog::Proc, (LPARAM)this); // pass obj pointer to proc
	if (!hDlg) {
		throw system_error(GetLastError(), std::system_category(), "CreateDialogParamW failed");
	}
	this->putWindowIcon(hDlg);
	ShowWindow(hDlg, cmdShow);

	HACCEL hAccel = this->accelId
		? LoadAccelerators(hInst, MAKEINTRESOURCE(this->accelId))
		: nullptr;
	return this->loop(hDlg, hAccel);
}

void MainDialog::putWindowIcon(HWND hDlg)
{
	if (!this->iconId) return;
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);

	SendMessage(hDlg, WM_SETICON, ICON_SMALL,
		(LPARAM)Icon{this->iconId, SIZE{16, 16}, optional{hInst}}.hIcon());
	SendMessage(hDlg, WM_SETICON, ICON_BIG,
		(LPARAM)Icon{this->iconId, SIZE{32, 32}, optional{hInst}}.hIcon());
}

int MainDialog::loop(HWND hDlg, HACCEL hAccel)
{
	MSG msg;

	for (;;) {
		if (BOOL ret = GetMessage(&msg, nullptr, 0, 0); ret == -1) {
			throw system_error(GetLastError(), std::system_category(), "GetMessageW failed");
		} else if (ret == 0) { // WM_QUIT was sent, exit gracefully
			break;
		}

		HWND hTopLevel = GetAncestor(hDlg, GA_ROOT);
		if (hAccel && TranslateAccelerator(hTopLevel, hAccel, &msg)) {
			continue; // message translated, no further processing is done
		}

		if (IsDialogMessage(hDlg, &msg)) {
			continue; // processed all keyboard actions for child controls
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
