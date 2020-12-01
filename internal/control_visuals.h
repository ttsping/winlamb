/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <Windows.h>
#include <VsStyle.h>
#include <Uxtheme.h>
#include "../gdi.h"
#pragma comment(lib, "UxTheme.lib")

namespace _wli::control_visuals {

// "&He && she" becomes "He & she".
[[nodiscard]] inline std::wstring remove_accel_ampersands(std::wstring_view s)
{
	std::wstring ret;
	ret.reserve(s.length());

	for (size_t i = 0; i < s.length() - 1; ++i) {
		if (s[i] == L'&' && s[i + 1] != L'&') {
			continue;
		}
		ret += s[i];
	}
	if (s.back() != L'&') {
		ret += s.back();
	}
	return ret;
}

// Calculates the bound rectangle to fit the text with current system font.
[[nodiscard]] inline SIZE calc_text_bound_box(HWND hReference,
	std::wstring_view text, bool considerAccelerators)
{
	if (hReference == nullptr) {
		throw std::logic_error("No reference HWND to calc text bound box.");
	}

	static std::wstring bufText;

	if (text.empty()) {
		bufText = L"Pj"; // placeholder to calc text height
	} else if (considerAccelerators) {
		bufText = remove_accel_ampersands(text);
	} else {
		bufText = text;
	}

	wl::gdi::dc_get referenceDC{hReference};
	wl::gdi::dc_compatible cloneDC{referenceDC};
	cloneDC.select_object(globalUiFont);
	SIZE bounds = cloneDC.get_text_extent_point(bufText);

	if (text.empty()) {
		bounds.cx = 0; // if no text was given, return height only
	}
	return bounds;
}

// Calculates the bound rectangle of a check box or radio button.
[[nodiscard]] inline SIZE calc_check_bound_box(HWND hParent, std::wstring_view text)
{
	SIZE bounds = calc_text_bound_box(hParent, text, true);
	bounds.cx += GetSystemMetrics(SM_CXMENUCHECK)
		+ GetSystemMetrics(SM_CXEDGE); // https://stackoverflow.com/a/1165052/6923555

	int cyCheckMark = GetSystemMetrics(SM_CXMENUCHECK);
	if (cyCheckMark > bounds.cy) {
		bounds.cy = cyCheckMark; // if the check is taller than the font, use its height
	}

	return bounds;
}

// Multiples values by current system DPI.
template<typename T,
	typename = std::enable_if_t<
		std::is_same_v<T, POINT> ||
		std::is_same_v<T, SIZE>
	>>
[[nodiscard]] inline T multiply_dpi(const T& val)
{
	// POINT and SIZE have fields with the same sizes.
	const POINT* pt = reinterpret_cast<const POINT*>(&val);
	if (pt->x == 0 && pt->y == 0) return {0, 0}; // nothing to do

	static int xDpi = 0, yDpi = 0;

	if (xDpi == 0) { // not initialized yet
		wl::gdi::dc_get screenDC{nullptr};
		xDpi = screenDC.get_device_caps(LOGPIXELSX); // cache
		yDpi = screenDC.get_device_caps(LOGPIXELSY);
	}

	// Return a new POINT or SIZE object.
	return {
		MulDiv(pt->x, xDpi, 96),
		MulDiv(pt->y, yDpi, 96)
	};
}

// Paints the themed border of an user control, if it has the proper styles.
inline void paint_control_borders(HWND hWnd, WPARAM wp, LPARAM lp) noexcept
{
	DefWindowProcW(hWnd, WM_NCPAINT, wp, lp); // make system draw the scrollbar for us

	if ((GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE) == 0 || // no border
		!IsThemeActive() ||
		!IsAppThemed() ) return;

	RECT rc{};
	GetWindowRect(hWnd, &rc); // window outmost coordinates, including margins
	ScreenToClient(hWnd, reinterpret_cast<POINT*>(&rc));
	ScreenToClient(hWnd, reinterpret_cast<POINT*>(&rc.right));
	rc.left += 2; rc.top += 2; rc.right += 2; rc.bottom += 2; // because it comes up anchored at -2,-2

	RECT rc2{}; // clipping region; will draw only within this rectangle
	wl::gdi::dc_get_window wndDc{hWnd};
	HTHEME hTheme = OpenThemeData(hWnd, L"LISTVIEW"); // borrow style from listview
	if (hTheme != nullptr) {
		SetRect(&rc2, rc.left, rc.top, rc.left + 2, rc.bottom); // draw only the borders to avoid flickering
		DrawThemeBackground(hTheme, wndDc.hdc(), LVP_LISTGROUP, 0, &rc, &rc2); // draw themed left border
		SetRect(&rc2, rc.left, rc.top, rc.right, rc.top + 2);
		DrawThemeBackground(hTheme, wndDc.hdc(), LVP_LISTGROUP, 0, &rc, &rc2); // draw themed top border
		SetRect(&rc2, rc.right - 2, rc.top, rc.right, rc.bottom);
		DrawThemeBackground(hTheme, wndDc.hdc(), LVP_LISTGROUP, 0, &rc, &rc2); // draw themed right border
		SetRect(&rc2, rc.left, rc.bottom - 2, rc.right, rc.bottom);
		DrawThemeBackground(hTheme, wndDc.hdc(), LVP_LISTGROUP, 0, &rc, &rc2); // draw themed bottom border

		CloseThemeData(hTheme);
	}
}

}//namespace _wli::control_visuals