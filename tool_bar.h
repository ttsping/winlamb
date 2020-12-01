/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <string_view>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"
#include "image_list.h"

namespace wl {

/// Native tool bar control.
///
/// #include <tool_bar.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/toolbar-controls-overview
class tool_bar final : public i_control {
private:
	_wli::base_native_control _base;
	msg::tool_bar::wnd_events _events;

public:
	/// Constructor, explicitly defining the control ID.
	tool_bar(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit tool_bar(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	tool_bar(tool_bar&&) = default;

	/// Move assignment operator.
	tool_bar& operator=(tool_bar&&) = default;

	/// Calls CreateWindowEx().
	/// @note Should be called during parent's WM_CREATE processing (or if
	/// dialog, WM_INITDIALOG).
	/// @param tbStyles Tool bar control styles. TBSTYLE_ and CCS_ constants. If
	/// the tool bar will be inserted into a wl::rebar, must include CCS_NODIVIDER
	/// and CCS_NOPARENTALIGN.
	/// @param tbExStyles Extended tool bar control styles. TBSTYLE_EX_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/toolbar-control-and-button-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/toolbar-extended-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/common-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	tool_bar& create(
		DWORD tbStyles = TBSTYLE_LIST | TBSTYLE_TRANSPARENT,
		DWORD tbExStyles = 0,
		DWORD wStyles = WS_CHILD,
		DWORD wExStyles = 0)
	{
		this->_base.create_window(TOOLBARCLASSNAMEW, {}, {}, {}, tbStyles | wStyles, wExStyles);
		SendMessageW(this->hwnd(), TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
		if (tbExStyles != 0) {
			this->set_extended_style(true, tbExStyles);
		}
		return *this;
	}

	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::tool_bar::wnd_events& on() noexcept { return this->_events; }

	/// Exposes the subclassing handler methods. If at least one handle is added, the control will be subclassed.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/subclassing-overview
	[[nodiscard]] msg::wnd_events& on_subclass() { return this->_base.on_subclass(); }

	/// Returns the underlying HWND handle.
	[[nodiscard]] HWND hwnd() const noexcept override { return this->_base.hwnd(); }

	/// Returns the control ID.
	[[nodiscard]] WORD ctrl_id() const noexcept { return this->_base.ctrl_id(); } // no override, doesn't inherit from i_control

	/// Adds a button with an icon.
	/// @note Before calling this method, call set_image_list() to make the icons available to the tool bar.
	/// @param text The text of the button.
	/// @param cmdId The command ID that will be sent in a WM_COMMAND message when the button is clicked.
	/// @param iIcon Zero-based index of the icon, from the associated wl::image_list, to be displayed.
	/// @param btnState State flags of the button. TBSTATE_ constants.
	/// @param btnStyle Style flags of the button. BTNS_ constants.
	/// @param lp Custom data to be associated to the button.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tb-addbuttons
	const tool_bar& add_button(std::wstring_view text, WORD cmdId, int iIcon,
		BYTE btnState = TBSTATE_ENABLED,
		BYTE btnStyle = BTNS_AUTOSIZE,
		LPARAM lp = 0) const
	{
		TBBUTTON tbb{};
		tbb.iString = reinterpret_cast<INT_PTR>(text.data());
		tbb.idCommand = cmdId;
		tbb.iBitmap = iIcon;
		tbb.fsState = btnState;
		tbb.fsStyle = btnStyle;
		tbb.dwData = lp;

		return this->_add_button(tbb);
	}

	/// Adds a separator gap.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tb-addbuttons
	const tool_bar& add_separator() const
	{
		TBBUTTON tbb{};
		tbb.fsStyle = BTNS_SEP;

		return this->_add_button(tbb);
	}

	/// Retrieves the total number of buttons and separators in the tool bar.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tb-buttoncount
	size_t count() const noexcept { return SendMessageW(this->hwnd(), TB_BUTTONCOUNT, 0, 0); }

	/// Enables or disables a button.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tb-enablebutton
	const tool_bar& enable_button(WORD cmdId, bool isEnabled) const
	{
		if (SendMessageW(this->hwnd(), TB_ENABLEBUTTON,
			cmdId, MAKELPARAM(isEnabled, 0)) == FALSE)
		{
			throw std::runtime_error("TB_ENABLEBUTTON failed in " __FUNCTION__ "().");
		}
		return *this;
	}

	/// Retrieves the TBSTYLE_EX_ styles.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tb-getextendedstyle
	[[nodiscard]] DWORD extended_style() const noexcept
	{
		return static_cast<DWORD>(SendMessageW(this->hwnd(), TB_GETEXTENDEDSTYLE, 0, 0));
	}

	/// Sets or unsets tool bar extended styles
	/// @param isSet Tells if the flags will be set or unset.
	/// @param tbExStyles Extended list view control styles. TBSTYLE_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tb-setextendedstyle
	const tool_bar& set_extended_style(bool isSet, DWORD tbExStyles) const noexcept
	{
		DWORD curStyles = this->extended_style();
		SendMessageW(this->hwnd(), TB_SETEXTENDEDSTYLE,
			0, isSet ? (curStyles | tbExStyles) : (curStyles & ~tbExStyles));
		return *this;
	}

	/// Sets the associated wl::image_list.
	/// The image list object is shared, and must remain valid.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tb-setimagelist
	const tool_bar& set_image_list(const wl::image_list& imgList) const noexcept
	{
		SendMessageW(this->hwnd(), TB_SETIMAGELIST,
			0, reinterpret_cast<LPARAM>(imgList.himagelist()));
		return *this;
	}

private:
	const tool_bar& _add_button(const TBBUTTON& tbb) const
	{
		if (SendMessageW(this->hwnd(), TB_ADDBUTTONS,
			1, reinterpret_cast<LPARAM>(&tbb)) == FALSE)
		{
			throw std::runtime_error("TB_ADDBUTTONS failed in " __FUNCTION__ "().");
		}
		SendMessageW(this->hwnd(), TB_AUTOSIZE, 0, 0);
		ShowWindow(this->hwnd(), TRUE); // will become visible if not yet
		return *this;
	}
};

}//namespace wl