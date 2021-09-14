
#pragma once
#include <Windows.h>

namespace core {

class MainDialog {
private:
	HWND hDlg;

public:
	HINSTANCE hInst = nullptr;
	int dlgId = 0;
	int iconId = 0;
	int cmdShow = SW_SHOW;
	DLGPROC dlgProc = nullptr;

	int run();

private:
	void setIcon();
	int loop();
};

}
