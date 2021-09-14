
#include <cwchar>
#include <string>
#include "MainDialog.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")
using namespace core;

int MainDialog::run()
{
	InitCommonControls();
	this->hDlg = CreateDialogParamW(this->hInst,
		MAKEINTRESOURCEW(this->dlgId), nullptr, this->dlgProc, 0);
	this->setIcon();
	ShowWindow(this->hDlg, this->cmdShow);
	return this->loop();
}

void MainDialog::setIcon()
{
	SendMessageW(this->hDlg, WM_SETICON, ICON_SMALL,
		(LPARAM)(HICON)LoadImageW(this->hInst,
			MAKEINTRESOURCEW(this->iconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	SendMessageW(this->hDlg, WM_SETICON, ICON_BIG,
		(LPARAM)(HICON)LoadImageW(this->hInst,
			MAKEINTRESOURCEW(this->iconId), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
}

int MainDialog::loop()
{
	MSG msg;
	BOOL ret = FALSE;

	while ((ret = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
		if (ret == -1) {
			wchar_t buf[60];
			std::swprintf(buf, ARRAYSIZE(buf), L"GetMessage error: %d.", GetLastError());
			MessageBoxW(nullptr, buf, L"Error", MB_ICONERROR);
		}

		if (IsDialogMessageW(this->hDlg, &msg)) {
			continue;
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}
