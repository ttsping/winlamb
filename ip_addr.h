/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <array>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native IP address control.
///
/// #include <ip_addr.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ip-address-controls
class ip_addr final : public i_resizable_control, public i_resource_control {
private:
	_wli::base_native_control _base;
	msg::ip_addr::wnd_events _events;

public:
	/// Constructor, explicitly defining the control ID.
	ip_addr(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit ip_addr(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	ip_addr(ip_addr&&) = default;

	/// Move assignment operator.
	ip_addr& operator=(ip_addr&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	ip_addr& create(POINT pos, SIZE size,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(WC_IPADDRESSW, L"", pos, size, wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		return *this;
	}

	/// Calls CreateWindowEx(), with a standard height of 21 pixels.
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param width Width of the control. Will be adjusted to match current system DPI.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	ip_addr& create(POINT pos, UINT width,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		return this->create(pos, {static_cast<LONG>(width), 21}, wStyles, wExStyles);
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::ip_addr::wnd_events& on() noexcept { return this->_events; }

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
	const ip_addr& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Tells if the address is blank.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ipm-isblank
	bool blank() const noexcept { return SendMessageW(this->hwnd(), IPM_ISBLANK, 0, 0) != 0; }

	/// Clears the address.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ipm-clearaddress
	const ip_addr& clear() const noexcept
	{
		SendMessageW(this->hwnd(), IPM_CLEARADDRESS, 0, 0);
		return *this;
	}

	/// Sets the IP address.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ipm-setaddress
	const ip_addr& set_addr(std::array<BYTE, 4> newAddr) const noexcept
	{
		SendMessageW(this->hwnd(), IPM_SETADDRESS, 0,
			MAKEIPADDRESS(newAddr[0], newAddr[1], newAddr[2], newAddr[3]) );
		return *this;
	}

	/// Retrieves the IP address.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ipm-getaddress
	std::array<BYTE, 4> addr() const noexcept
	{
		DWORD buf = 0;
		SendMessageW(this->hwnd(), IPM_GETADDRESS, 0, reinterpret_cast<LPARAM>(&buf));
		return {FIRST_IPADDRESS(buf), SECOND_IPADDRESS(buf),
			THIRD_IPADDRESS(buf), FOURTH_IPADDRESS(buf)};
	}

	/// Sets the focus to one of the 4 fields.
	///
	/// @param field A zero-based field index to which the focus should be set.
	/// If this value is greater than the number of fields, focus is set to the first
	/// blank field. If all fields are nonblank, focus is set to the first field.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ipm-setfocus
	const ip_addr& focus_field(BYTE field) const
	{
		SendMessageW(this->hwnd(), IPM_SETFOCUS, field, 0);
		return *this;
	}
};

}//namespace wl