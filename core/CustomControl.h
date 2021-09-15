
#pragma once
#include <Windows.h>

namespace core {

class CustomControl final {
public:
	// Implement this interface to handle messages from a CustomControl object.
	class Handler {
	public:
		virtual LRESULT windowProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) = 0;
		virtual void create(HWND hParent, int x, int y, int cx, int cy) = 0;
	};

private:
	Handler* handler;
	DWORD classStyles;
	HCURSOR hCursor;
	HBRUSH hBrushBg;
	DWORD wndExStyles, wndStyles;

public:
	CustomControl(Handler* handler, DWORD classStyles, HCURSOR hCursor, HBRUSH hBrushBg,
		DWORD wndExStyles, DWORD wndStyles)
		: handler{handler}, classStyles{classStyles}, hCursor{hCursor}, hBrushBg{hBrushBg},
			wndExStyles{wndExStyles}, wndStyles{wndStyles} {}
	void create(HWND hParent, int x, int y, int cx, int cy);

private:
	static LRESULT CALLBACK Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

}
