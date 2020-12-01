/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <chrono>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"
#include "time.h"

namespace wl {

/// Native month calendar control.
///
/// #include <month_calendar.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/month-calendar-controls
class month_calendar final : public i_resizable_control, public i_resource_control {
private:
	_wli::base_native_control _base;
	msg::month_calendar::wnd_events _events;

public:
	/// Constructor, explicitly defining the control ID.
	month_calendar(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit month_calendar(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	month_calendar(month_calendar&&) = default;

	/// Move assignment operator.
	month_calendar& operator=(month_calendar&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param mcStyles Month calendar control styles. MCS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/month-calendar-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	month_calendar& create(POINT pos,
		DWORD mcStyles = 0,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);

		this->_base.create_window(MONTHCAL_CLASSW, {}, pos, {0, 0},
			mcStyles | wStyles, wExStyles);

		RECT rc{};
		MonthCal_GetMinReqRect(this->hwnd(), &rc);
		SetWindowPos(this->hwnd(), nullptr, 0, 0,
			rc.right, rc.bottom, SWP_NOZORDER | SWP_NOMOVE);

		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::month_calendar::wnd_events& on() noexcept { return this->_events; }

	/// Exposes the subclassing handler methods. If at least one handle is added, the control will be subclassed.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/subclassing-overview
	[[nodiscard]] msg::wnd_events& on_subclass() { return this->_base.on_subclass(); }

	/// Returns the underlying HWND handle.
	[[nodiscard]] HWND hwnd() const noexcept override { return this->_base.hwnd(); }

	/// Returns the control ID.
	[[nodiscard]] WORD ctrl_id() const noexcept override { return this->_base.ctrl_id(); }

	/// Sets the selected date.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-monthcal_setcursel
	const month_calendar& set_selected_date(
		std::chrono::system_clock::time_point newSel) const
	{
		SYSTEMTIME st = time::timepoint_to_systemtime(newSel);
		MonthCal_SetCurSel(this->hwnd(), &st);
		return *this;
	}

	/// Sets the selected date.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-monthcal_setcursel
	const month_calendar& set_selected_date(const SYSTEMTIME& newSel) const
	{
		MonthCal_SetCurSel(this->hwnd(), &newSel);
		return *this;
	}

	/// Retrieves the selected date.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-monthcal_getcursel
	[[nodiscard]] std::chrono::system_clock::time_point selected_date() const
	{
		SYSTEMTIME st{};
		MonthCal_GetCurSel(this->hwnd(), &st);
		return time::systemtime_to_timepoint(st);
	}
};

}//namespace wl