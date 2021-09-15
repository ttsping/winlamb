
#pragma once
#include <Windows.h>

namespace core {

// Creates and manages the main window of the application, created from a dialog resource.
class MainDialog final {
public:
	// Implement this interface to handle messages from a MainDialog object.
	class Handler {
	public:
		virtual INT_PTR dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) = 0;
		virtual int run(HINSTANCE hInst, int cmdShow) = 0;
	};

private:
	static HFONT hFontSys;
	Handler* handler = nullptr;
	int dialogId = 0;
	int iconId = 0;
	int accelId = 0;

public:
	~MainDialog();
	MainDialog(Handler* handler, int dialogId, int iconId, int accelId)
		: handler{handler}, dialogId{dialogId}, iconId{iconId}, accelId{accelId} { }

	int run(HINSTANCE hInst, int cmdShow);
	static HFONT UiFont() { return MainDialog::hFontSys; }

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
