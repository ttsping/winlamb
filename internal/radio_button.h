/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <string_view>
#include <Windows.h>
#include "base_native_control.h"
#include "control_visuals.h"
#include "gdi_obj.h"
#include "interfaces.h"
#include "msg_wnd_events_ctrl.h"

namespace wl {

class radio_group; // forward declaration

/// A single radio button control.
/// A radio button is just a specific type of wl::button, so they share the same events.
///
/// #include <radio_group.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/button-types-and-styles#radio-buttons
class radio_button final : public i_resizable_control, public i_resource_control {
private:
	friend radio_group;
	_wli::base_native_control _base;
	msg::button::wnd_events _events;

public:
	/// Constructor, explicitly defining the control ID.
	radio_button(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit radio_button(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	radio_button(radio_button&&) = default;

	/// Move assignment operator.
	radio_button& operator=(radio_button&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param text The radio button text.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param bStyles Button control styles. BS_ constants.
	/// @param wStyles Window styles. WS_ constants. The first radio of a group must have WS_TABSTOP and WS_GROUP.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/button-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	radio_button& create(std::wstring_view text, POINT pos,
		DWORD bStyles = BS_AUTORADIOBUTTON,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		SIZE size = _wli::control_visuals::calc_check_bound_box(this->_base.parent().hwnd(), text);

		this->_base.create_window(L"BUTTON", text, pos, size, bStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::button::wnd_events& on() noexcept { return this->_events; }

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
	const radio_button& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Sets the text in this control.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowtextw
	const radio_button& set_text(std::wstring_view text) const
	{
		SIZE newSize = _wli::control_visuals::calc_check_bound_box(GetParent(this->hwnd()), text);
		SetWindowPos(this->hwnd(), nullptr, 0, 0,
			newSize.cx, newSize.cy, SWP_NOZORDER | SWP_NOMOVE);

		SetWindowTextW(this->hwnd(), text.data());
		return *this;
	}

	/// Retrieves the text in this control.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtextw
	[[nodiscard]] std::wstring text() const { return _wli::str_aux::get_window_text(this->hwnd()); }

	/// Sets the state to BST_CHECKED or BST_UNCHECKED.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bm-setcheck
	const radio_button& set_checked(bool isChecked) const noexcept
	{
		SendMessageW(this->hwnd(), BM_SETCHECK,
			isChecked ? BST_CHECKED : BST_UNCHECKED, 0);
		return *this;
	}

	/// Calls set_checked() and sends a WM_COMMAND message emulating the user click.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bm-setcheck
	const radio_button& set_checked_and_trigger(bool isChecked) const noexcept
	{
		this->set_checked(isChecked);
		SendMessageW(GetParent(this->hwnd()), WM_COMMAND,
			MAKEWPARAM(this->ctrl_id(), 0),
			reinterpret_cast<LPARAM>(this->hwnd()) );
		return *this;
	}

	/// Tells if current state is BST_CHECKED.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bm-getcheck
	[[nodiscard]] bool checked() const noexcept
	{
		return SendMessageW(this->hwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED;
	}
};

}//namespace wl