
#include <string>
#include "MainDialog.h"
#include <CommCtrl.h>
#include "../core/str.h"
#pragma comment(lib, "Comctl32.lib")
using namespace core;
using std::wstring;

int MainDialog::run(HINSTANCE hInst, int cmdShow)
{
	InitCommonControls();	

	HWND hDlg = CreateDialogParamW(hInst, MAKEINTRESOURCEW(this->dialogId),
		nullptr, core_internals::Dialog::Proc, (LPARAM)this); // pass handler obj to proc
	if (!hDlg) return GetLastError();
	this->putWindowIcon(hDlg);
	ShowWindow(hDlg, cmdShow);

	HACCEL hAccel = this->accelId
		? LoadAcceleratorsW(hInst, MAKEINTRESOURCEW(this->accelId))
		: nullptr;
	return this->loop(hDlg, hAccel);
}

void MainDialog::putWindowIcon(HWND hDlg)
{
	if (this->iconId) {
		HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
		SendMessageW(hDlg, WM_SETICON, ICON_SMALL,
			(LPARAM)(HICON)LoadImageW(hInst,
				MAKEINTRESOURCEW(this->iconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
		SendMessageW(hDlg, WM_SETICON, ICON_BIG,
			(LPARAM)(HICON)LoadImageW(hInst,
				MAKEINTRESOURCEW(this->iconId), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
	}
}

int MainDialog::loop(HWND hDlg, HACCEL hAccel)
{
	MSG msg;
	BOOL ret = FALSE;

	while ((ret = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
		if (ret == -1) {
			DWORD err = GetLastError();
			wstring errMsg = str::FormatError(err);

			wstring fullMsg = str::Format(L"GetMessage failed: %s.", errMsg.c_str());
			MessageBoxW(nullptr, fullMsg.c_str(), L"Loop error", MB_ICONERROR);
			return err;

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
