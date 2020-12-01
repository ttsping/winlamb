/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <chrono>
#include <string_view>
#include <Windows.h>
#include <CommCtrl.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/gdi_obj.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"
#include "time.h"

namespace wl {

/// Native date and time picker control.
///
/// #include <date_time_picker.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/date-and-time-picker-controls
class date_time_picker : public i_resizable_control, public i_resource_control {
private:
	_wli::base_native_control _base;
	msg::date_time_picker::wnd_events _events;

public:
	/// Constructor, explicitly defining the control ID.
	date_time_picker(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit date_time_picker(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	date_time_picker(date_time_picker&&) = default;

	/// Move assignment operator.
	date_time_picker& operator=(date_time_picker&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param dtStyles Date time picker control styles. DTS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/date-and-time-picker-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	date_time_picker& create(POINT pos, SIZE size,
		DWORD dtStyles = 0,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(DATETIMEPICK_CLASSW, {}, pos, size, dtStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		return *this;
	}

	/// Calls CreateWindowEx(), with a standard height of 21 pixels.
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param text The button text.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param width Width of the control. Will be adjusted to match current system DPI.
	/// @param dtStyles Date time picker control styles. DTS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/date-and-time-picker-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	date_time_picker& create(POINT pos, UINT width,
		DWORD dtStyles = 0,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		return this->create(pos, {static_cast<LONG>(width), 21},
			dtStyles, wStyles, wExStyles);
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::date_time_picker::wnd_events& on() noexcept { return this->_events; }

	/// Exposes the subclassing handler methods. If at least one handle is added, the control will be subclassed.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/subclassing-overview
	[[nodiscard]] msg::wnd_events& on_subclass() { return this->_base.on_subclass(); }

	/// Returns the underlying HWND handle.
	[[nodiscard]] HWND hwnd() const noexcept override { return this->_base.hwnd(); }

	/// Returns the control ID.
	[[nodiscard]] WORD ctrl_id() const noexcept override { return this->_base.ctrl_id(); }

	/// Calls EnableWindow().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enablewindow
	const date_time_picker& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Sets the time.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-datetime_setsystemtime
	const date_time_picker& set_time(
		std::chrono::system_clock::time_point newTime) const
	{
		SYSTEMTIME st = time::timepoint_to_systemtime(newTime);
		DateTime_SetSystemtime(this->hwnd(), GDT_VALID, &st);
		return *this;
	}

	/// Sets the time.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-datetime_setsystemtime
	const date_time_picker& set_time(const SYSTEMTIME& newTime) const
	{
		DateTime_SetSystemtime(this->hwnd(), GDT_VALID, &newTime);
		return *this;
	}

	/// Retrieves the time.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-datetime_getsystemtime
	[[nodiscard]] std::chrono::system_clock::time_point time() const
	{
		SYSTEMTIME st{};
		DateTime_GetSystemtime(this->hwnd(), &st);
		return time::systemtime_to_timepoint(st);
	}

	/// Sets the displaying format for the control.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-datetime_setformat
	const date_time_picker& set_format(std::wstring_view fmtStr) const noexcept
	{
		DateTime_SetFormat(this->hwnd(), fmtStr.data());
		return *this;
	}
};

}//namespace wl