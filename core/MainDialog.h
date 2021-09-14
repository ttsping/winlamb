
#pragma once
#include <Windows.h>
#include "DialogHandler.h"

namespace core {

class MainDialog final {
public:
	struct MainDialogSetup {
		DialogHandler* handler = nullptr;
		int dialogId = 0;
		int iconId = 0;
	};

	MainDialogSetup setup;
	int run(HINSTANCE hInst, int cmdShow);

private:
	void setIcon(HWND hDlg);
	int loop(HWND hDlg);
	static INT_PTR DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
};

}

#define RUN(handlerClass) \
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow) { \
	handlerClass window; \
	return window.run(hInst, cmdShow); \
}
