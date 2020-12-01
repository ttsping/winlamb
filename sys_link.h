/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native SysLink control, which renders text with hyperlink tags.
///
/// #include <sys_link.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/syslink-control-entry
class sys_link final : public i_resizable_control, public i_resource_control {
private:
	_wli::base_native_control _base;
	msg::sys_link::wnd_events _events;

public:
	/// Constructor, explicitly defining the control ID.
	sys_link(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit sys_link(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	sys_link(sys_link&&) = default;

	/// Move assignment operator.
	sys_link& operator=(sys_link&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param text The SysLink text.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param lwStyles SysLink control styles. LWS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/syslink-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	sys_link& create(std::wstring_view text, POINT pos,
		DWORD lwStyles = LWS_TRANSPARENT,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);

		this->_base.create_window(WC_LINK, text, pos, {0, 0}, lwStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		this->_set_ideal_size();
		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::sys_link::wnd_events& on() noexcept { return this->_events; }

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
	const sys_link& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Sets the text in the control, and resizes it to fit.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowtextw
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lm-getidealsize
	const sys_link& set_text(std::wstring_view text) const
	{
		SetWindowTextW(this->hwnd(), text.data());
		this->_set_ideal_size();
		return *this;
	}

	/// Retrieves the text in this control.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtextw
	[[nodiscard]] std::wstring text() const { return _wli::str_aux::get_window_text(this->hwnd()); }

private:
	void _set_ideal_size() const
	{
		SIZE sz{};
		SendMessageW(this->hwnd(), LM_GETIDEALSIZE, 0, reinterpret_cast<LPARAM>(&sz));
		SetWindowPos(this->hwnd(), nullptr, 0, 0, sz.cx, sz.cy, SWP_NOZORDER | SWP_NOMOVE);
	}
};

}//namespace wl