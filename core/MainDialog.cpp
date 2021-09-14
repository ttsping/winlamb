
#include <cwchar>
#include <string>
#include "MainDialog.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")
using namespace core;

int MainDialog::run(HINSTANCE hInst, int cmdShow)
{
	InitCommonControls();
	HWND hDlg = CreateDialogParamW(hInst, MAKEINTRESOURCEW(this->setup.dialogId),
		nullptr, DialogProc, (LPARAM)this->setup.handler);
	this->setIcon(hDlg);
	ShowWindow(hDlg, cmdShow);
	return this->loop(hDlg);
}

void MainDialog::setIcon(HWND hDlg)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
	SendMessageW(hDlg, WM_SETICON, ICON_SMALL,
		(LPARAM)(HICON)LoadImageW(hInst,
			MAKEINTRESOURCEW(this->setup.iconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	SendMessageW(hDlg, WM_SETICON, ICON_BIG,
		(LPARAM)(HICON)LoadImageW(hInst,
			MAKEINTRESOURCEW(this->setup.iconId), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
}

int MainDialog::loop(HWND hDlg)
{
	MSG msg;
	BOOL ret = FALSE;

	while ((ret = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
		if (ret == -1) {
			wchar_t buf[60];
			std::swprintf(buf, ARRAYSIZE(buf), L"GetMessage error: %d.", GetLastError());
			MessageBoxW(nullptr, buf, L"Error", MB_ICONERROR);
		}

		if (IsDialogMessageW(hDlg, &msg)) {
			continue;
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
	} else {
		pHandler = (DialogHandler*)GetWindowLongPtrW(hDlg, DWLP_USER);
	}

	return pHandler ? pHandler->dialogProc(hDlg, msg, wp, lp) : FALSE;
}
