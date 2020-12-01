/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <string_view>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/gdi_obj.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native edit (textbox) control.
///
/// #include <edit.h>
/// @see @ref ex03
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/about-edit-controls
class edit final : public i_resizable_control, public i_resource_control {
private:
	_wli::base_native_control _base;
	msg::edit::wnd_events _events;

public:
	/// Constructor, explicitly defining the control ID.
	edit(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit edit(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	edit(edit&&) = default;

	/// Move assignment operator.
	edit& operator=(edit&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param text Initial edit text.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param eStyles Edit control styles. ES_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/edit-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	edit& create(std::wstring_view text, POINT pos, SIZE size,
		DWORD eStyles = ES_AUTOHSCROLL,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = WS_EX_CLIENTEDGE)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(L"EDIT", text, pos, size, eStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		return *this;
	}

	/// Calls CreateWindowEx(), with a standard height of 21 pixels.
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param text Initial edit text.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param width Width of the control. Will be adjusted to match current system DPI.
	/// @param esStyles Edit control styles. ES_ constants.
	/// @param wsStyles Window styles. WS_ constants.
	/// @param wsExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/edit-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	edit& create(std::wstring_view text, POINT pos, UINT width,
		DWORD esStyles = ES_AUTOHSCROLL,
		DWORD wsStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wsExStyles = WS_EX_CLIENTEDGE)
	{
		return this->create(text, pos, {static_cast<LONG>(width), 21},
			esStyles, wsStyles, wsExStyles);
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::edit::wnd_events& on() noexcept { return this->_events; }

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
	const edit& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Sets the text in this control.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowtextw
	const edit& set_text(std::wstring_view t) const noexcept { SetWindowTextW(this->hwnd(), t.data()); return *this; }

	/// Retrieves the text in this control.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtextw
	[[nodiscard]] std::wstring text() const { return _wli::str_aux::get_window_text(this->hwnd()); }

	/// Replaces the currently selected text with the given one, with EM_REPLACESEL.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/em-replacesel
	const edit& replace_selection(std::wstring_view replacement) const noexcept
	{
		SendMessageW(this->hwnd(), EM_REPLACESEL, TRUE,
			reinterpret_cast<LPARAM>(replacement.data()));
		return *this;
	}

	/// Selects all the text with EM_SETSEL.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/em-setsel
	const edit& select_all() const noexcept
	{
		SendMessageW(this->hwnd(), EM_SETSEL, 0, -1);
		return *this;
	}

	/// Returns the selected text with EM_GETSEL.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/em-getsel
	[[nodiscard]] std::wstring selection() const
	{
		DWORD start = 0, pastEnd = 0;
		SendMessageW(this->hwnd(), EM_GETSEL,
			reinterpret_cast<WPARAM>(&start), reinterpret_cast<LPARAM>(&pastEnd));
		std::wstring text = this->text();
		return text.substr(start, pastEnd - start);
	}

	/// Sets the selection range with EM_SETSEL.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/em-setsel
	const edit& set_selection(int start, int length) const noexcept
	{
		SendMessageW(this->hwnd(), EM_SETSEL, start, static_cast<size_t>(start) + length);
		return *this;
	}
};

}//namespace wl