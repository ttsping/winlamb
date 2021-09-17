
#pragma once
#include "Dialog.h"
#include "internals.h"

namespace core {

// Creates and manages the main window of the application, created from a dialog resource.
class MainDialog : public Dialog {
private:
	int iconId;
	int accelId;

public:
	virtual ~MainDialog() { }

	MainDialog(int dialogId, int iconId, int accelId)
		: Dialog{dialogId}, iconId{iconId}, accelId{accelId} { }

	virtual int run(HINSTANCE hInst, int cmdShow);

private:
	void putWindowIcon(HWND hDlg);
	int loop(HWND hDlg, HACCEL hAccel);
};

}

// Implements the WinMain() entry point and instantiates the main dialog object.
#define RUN(wndClass) \
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow) { \
	int ret = 0; \
	try { \
		wndClass window; \
		ret = window.run(hInst, cmdShow); \
	} catch (...) { \
		core_internals::Lippincott(); \
		ret = -1; \
	} \
	return ret; \
}
