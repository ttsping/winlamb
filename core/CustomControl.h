
#pragma once
#include "Window.h"

namespace core {

class CustomControl : public Window {
private:
	DWORD classStyles;
	HCURSOR hCursor;
	HBRUSH hBrushBg;
	DWORD wndExStyles, wndStyles;
	WORD ctlId;

public:
	virtual ~CustomControl() { }

	CustomControl(DWORD classStyles, HCURSOR hCursor, HBRUSH hBrushBg,
		DWORD wndExStyles, DWORD wndStyles)
		: classStyles{classStyles}, hCursor{hCursor}, hBrushBg{hBrushBg},
			wndExStyles{wndExStyles}, wndStyles{wndStyles}, ctlId{NextCtrlId()} {}

	virtual LRESULT windowProc(UINT msg, WPARAM wp, LPARAM lp) = 0;
	virtual void create(Window* parent, int x, int y, int cx, int cy);
	[[nodiscard]] constexpr WORD ctrlId() const { return this->ctlId; }

private:
	[[nodiscard]] static WORD NextCtrlId();
	static LRESULT CALLBACK Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	static LRESULT PaintThemeBorders(HWND hWnd, WPARAM wp, LPARAM lp);
};

}
