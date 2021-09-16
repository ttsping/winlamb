
#pragma once
#include <Windows.h>
#include "Dialog.h"

namespace core {

// Creates and manages the main window of the application, created from a dialog resource.
class MainDialog : public Dialog {
private:
	int dialogId;
	int iconId;
	int accelId;

public:
	MainDialog(int dialogId, int iconId, int accelId)
		: dialogId{dialogId}, iconId{iconId}, accelId{accelId} { }

	virtual int run(HINSTANCE hInst, int cmdShow);

private:	
	void putWindowIcon(HWND hDlg);
	int loop(HWND hDlg, HACCEL hAccel);
};

}

// Implements the WinMain() entry point and instantiates the main dialog object.
#define RUN(handlerClass) \
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow) { \
	handlerClass window; \
	return window.run(hInst, cmdShow); \
}
