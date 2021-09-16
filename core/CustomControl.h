
#pragma once
#include <Windows.h>
#include "Window.h"

namespace core {

class CustomControl : public Window {
private:
	DWORD classStyles;
	HCURSOR hCursor;
	HBRUSH hBrushBg;
	DWORD wndExStyles, wndStyles;

public:
	virtual LRESULT windowProc(UINT msg, WPARAM wp, LPARAM lp) = 0;

	CustomControl(DWORD classStyles, HCURSOR hCursor, HBRUSH hBrushBg,
		DWORD wndExStyles, DWORD wndStyles)
		: classStyles{classStyles}, hCursor{hCursor}, hBrushBg{hBrushBg},
			wndExStyles{wndExStyles}, wndStyles{wndStyles} {}

	virtual void create(Window* parent, int x, int y, int cx, int cy);

private:
	static LRESULT CALLBACK Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

}
