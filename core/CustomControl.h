
#pragma once
#include <optional>
#include "WindowThread.h"

namespace core {

// Manages a custom child control.
class CustomControl : public WindowThread {
private:
	DWORD classStyles;
	HCURSOR hCursor;
	HBRUSH hBrushBg;
	DWORD wndExStyles, wndStyles;
	WORD ctlId;

public:
	virtual ~CustomControl() { }

	CustomControl(DWORD classStyles, HCURSOR hCursor, HBRUSH hBrushBg,
		DWORD wndExStyles, DWORD wndStyles,
		std::optional<WORD> ctlId = std::nullopt) noexcept
		: WindowThread{},
			classStyles{classStyles}, hCursor{hCursor}, hBrushBg{hBrushBg},
			wndExStyles{wndExStyles}, wndStyles{wndStyles},
			ctlId{ctlId ? *ctlId : NextCtrlId()} { }

	virtual LRESULT windowProc(UINT msg, WPARAM wp, LPARAM lp) = 0;
	virtual void create(Window* parent, int x, int y, int cx, int cy);
	[[nodiscard]] constexpr WORD ctrlId() const noexcept { return this->ctlId; }

private:
	[[nodiscard]] static WORD NextCtrlId() noexcept;
	static LRESULT CALLBACK Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) noexcept;
	[[nodiscard]] static bool PaintThemeBorders(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) noexcept;
};

}
