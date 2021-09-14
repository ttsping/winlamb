
#include <cwchar>
#include <string>
#include "MainDialog.h"
#include <CommCtrl.h>
#include <VersionHelpers.h>
#pragma comment(lib, "Comctl32.lib")
using namespace core;

HFONT MainDialog::hFontSys = nullptr;

MainDialog::~MainDialog()
{
	if (MainDialog::hFontSys) DeleteObject(MainDialog::hFontSys);
}

int MainDialog::run(HINSTANCE hInst, int cmdShow)
{
	InitCommonControls();

	NONCLIENTMETRICS ncm = {0};
	ncm.cbSize = sizeof(ncm);
	if (!IsWindowsVistaOrGreater()) ncm.cbSize -= sizeof(int);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	if (!(MainDialog::hFontSys = CreateFontIndirectW(&ncm.lfMenuFont))) {
		return GetLastError();
	}

	HWND hDlg = CreateDialogParamW(hInst, MAKEINTRESOURCEW(this->setup.dialogId),
		nullptr, DialogProc, (LPARAM)this->setup.handler);
	this->putWindowIcon(hDlg);
	ShowWindow(hDlg, cmdShow);

	HACCEL hAccel = this->setup.accelId
		? LoadAcceleratorsW(hInst, MAKEINTRESOURCEW(this->setup.accelId))
		: nullptr;
	return this->loop(hDlg, hAccel);
}

void MainDialog::putWindowIcon(HWND hDlg)
{
	if (this->setup.iconId) {
		HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
		SendMessageW(hDlg, WM_SETICON, ICON_SMALL,
			(LPARAM)(HICON)LoadImageW(hInst,
				MAKEINTRESOURCEW(this->setup.iconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
		SendMessageW(hDlg, WM_SETICON, ICON_BIG,
			(LPARAM)(HICON)LoadImageW(hInst,
				MAKEINTRESOURCEW(this->setup.iconId), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
	}
}

int MainDialog::loop(HWND hDlg, HACCEL hAccel)
{
	MSG msg;
	BOOL ret = FALSE;

	while ((ret = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
		if (ret == -1) {
			DWORD err = GetLastError();
			wchar_t buf[60];
			std::swprintf(buf, ARRAYSIZE(buf), L"GetMessage error: %d.", err);
			MessageBoxW(nullptr, buf, L"Error", MB_ICONERROR);
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

INT_PTR MainDialog::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	DialogHandler* pHandler;

	if (msg == WM_INITDIALOG) {
		pHandler = (DialogHandler*)lp;
		SetWindowLongPtrW(hDlg, DWLP_USER, (LONG_PTR)pHandler);
		EnumChildWindows(hDlg, [](HWND hCtrl, LPARAM lp) -> BOOL {
			SendMessageW(hCtrl, WM_SETFONT, (WPARAM)(HFONT)lp, MAKELPARAM(FALSE, 0));
			return TRUE;
		}, (LPARAM)MainDialog::hFontSys);
	} else {
		pHandler = (DialogHandler*)GetWindowLongPtrW(hDlg, DWLP_USER);
	}

	return pHandler ? pHandler->dialogProc(hDlg, msg, wp, lp) : FALSE;
}
