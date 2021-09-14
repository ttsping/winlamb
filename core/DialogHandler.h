
#pragma once
#include <Windows.h>

namespace core {

class DialogHandler {
public:
	virtual INT_PTR dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) = 0;
	virtual int run(HINSTANCE hInst, int cmdShow) = 0;
};

}
