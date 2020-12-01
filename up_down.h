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

/// Native up down control. Usually associated with a buddy wl::edit control.
///
/// #include <up_down.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/up-down-controls
class up_down final : public i_resizable_control, public i_resource_control {
private:
	_wli::base_native_control _base;
	msg::up_down::wnd_events _events;

public:
	/// The radix base used to represent the value in the buddy wl::edit control.
	enum class radix : DWORD {
		/// Decimal, range can be signed or unsigned.
		DEC = 10,
		/// Hexadecimal, range only be only unsigned.
		HEX = 16
	};

	/// Constructor, explicitly defining the control ID.
	up_down(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit up_down(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	up_down(up_down&&) = default;

	/// Move assignment operator.
	up_down& operator=(up_down&&) = default;

	/// Calls CreateWindowEx().
	///
	/// Usually the up down is associated with a wl::edit control. To do so,
	/// create an edit right before creating the up down, so the edit will take
	/// the previous z-order. Then create the up down with zero position/size and
	/// UDS_AUTOBUDDY style. The up down will be automatically positioned and
	/// sized within the edit.
	///
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param udStyles Up down control styles. UDS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/button-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/up-down-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	up_down& create(POINT pos, SIZE size,
		DWORD udStyles = UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK,
		DWORD wStyles = WS_CHILD | WS_VISIBLE,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(UPDOWN_CLASSW, {}, pos, size, udStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::up_down::wnd_events& on() noexcept { return this->_events; }

	/// Exposes the subclassing handler methods. If at least one handle is added, the control will be subclassed.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/subclassing-overview
	[[nodiscard]] msg::wnd_events& on_subclass() { return this->_base.on_subclass(); }

	/// Returns the underlying HWND handle.
	[[nodiscard]] HWND hwnd() const noexcept override { return this->_base.hwnd(); }

	/// Returns the control ID.
	[[nodiscard]] WORD ctrl_id() const noexcept override { return this->_base.ctrl_id(); }

	/// Retrieves the radix base.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/udm-getbase
	[[nodiscard]] radix radix_base() const noexcept
	{
		return static_cast<radix>(SendMessageW(this->hwnd(), UDM_GETBASE, 0, 0));
	}

	/// Retrieves the control range.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/udm-getrange32
	std::pair<int, int> range() const noexcept
	{
		std::pair<int, int> theRange{0, 0};
		SendMessageW(this->hwnd(), UDM_GETRANGE32,
			reinterpret_cast<WPARAM>(&theRange.first),
			reinterpret_cast<WPARAM>(&theRange.second));
		return theRange;
	}

	/// Sets the radix base.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/udm-setbase
	const up_down& set_radix_base(radix newBase) const noexcept
	{
		SendMessageW(this->hwnd(), UDM_SETBASE, static_cast<DWORD>(newBase), 0);
		return *this;
	}

	/// Sets the control range.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/udm-setrange32
	const up_down& set_range(int min, int max) const noexcept
	{
		SendMessageW(this->hwnd(), UDM_SETRANGE32, min, max);
		return *this;
	}
};

}//namespace wl