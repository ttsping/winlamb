
#pragma once
#include <Windows.h>

namespace core_internals {

class Dialog {
public:
	class Handler {
	public:
		virtual INT_PTR dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) = 0;
	};

	static INT_PTR Proc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
};

}
