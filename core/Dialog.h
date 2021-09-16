
#pragma once
#include <Windows.h>
#include "Window.h"

namespace core {

class MainDialog;
class ModalDialog;

// Keeps the dialog procedure. Base to all dialog-based windows.
class Dialog : public Window {
	friend core::MainDialog;
	friend core::ModalDialog;

private:
	int dialogId;

public:
	virtual ~Dialog() { }

	Dialog(int dialogId) : dialogId{dialogId} { }
	virtual INT_PTR dialogProc(UINT msg, WPARAM wp, LPARAM lp) = 0;

private:
	static INT_PTR CALLBACK Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
};

}
