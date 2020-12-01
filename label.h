/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <string_view>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/gdi_obj.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native static (label) control.
///
/// #include <label.h>
/// @see @ref ex03
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/about-static-controls
class label final : public i_resizable_control, public i_resource_control {
private:
	_wli::base_native_control _base;
	msg::label::wnd_events _events;

public:
	/// Constructor, explicitly defining the control ID.
	label(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit label(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	label(label&&) = default;

	/// Move assignment operator.
	label& operator=(label&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param text The label text.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param sStyles Static control styles. SS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/static-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	label& create(std::wstring_view text, POINT pos,
		DWORD sStyles = SS_LEFT,
		DWORD wStyles = WS_CHILD | WS_VISIBLE,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		SIZE size = _wli::control_visuals::calc_text_bound_box(this->_base.parent().hwnd(), text, true);

		this->_base.create_window(L"STATIC", text, pos, size, sStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::label::wnd_events& on() noexcept { return this->_events; }

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
	const label& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Sets the text in the control, and resizes it to fit.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowtextw
	const label& set_text(std::wstring_view text) const
	{
		SIZE newSize = _wli::control_visuals::calc_text_bound_box(GetParent(this->hwnd()), text, true);
		SetWindowPos(this->hwnd(), nullptr, 0, 0,
			newSize.cx, newSize.cy, SWP_NOZORDER | SWP_NOMOVE);

		SetWindowTextW(this->hwnd(), text.data());
		return *this;
	}

	/// Retrieves the text in this control.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtextw
	[[nodiscard]] std::wstring text() const { return _wli::str_aux::get_window_text(this->hwnd()); }
};

}//namespace wl