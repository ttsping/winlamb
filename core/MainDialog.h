
#pragma once
#include <Windows.h>
#include "DialogHandler.h"

namespace core {

class MainDialog final {
private:
	static HFONT hFontSys;
	DialogHandler* handler = nullptr;
	int dialogId = 0;
	int iconId = 0;
	int accelId = 0;

public:
	~MainDialog();
	MainDialog(DialogHandler* handler, int dialogId, int iconId, int accelId)
		: handler{handler}, dialogId{dialogId}, iconId{iconId}, accelId{accelId} { }

	static HFONT UiFont() { return MainDialog::hFontSys; }
	int run(HINSTANCE hInst, int cmdShow);

private:	
	void putWindowIcon(HWND hDlg);
	int loop(HWND hDlg, HACCEL hAccel);
	static INT_PTR DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
};

}

#define RUN(handlerClass) \
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow) { \
	handlerClass window; \
	return window.run(hInst, cmdShow); \
}
