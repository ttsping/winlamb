
#pragma once
#include <Windows.h>

namespace core {
	class MainDialog;
	class ModalDialog;
}

namespace core_internals {

class Dialog {
	friend core::MainDialog;
	friend core::ModalDialog;

public:
	virtual INT_PTR dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) = 0;

private:
	static INT_PTR CALLBACK Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
};

}
