/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native header control.
///
/// #include <header.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/header-controls
class header final : public i_resizable_control, public i_resource_control {
public:
	/// A single item of a wl::header.
	/// It's a simple object that can be cheaply copied.
	/// 
	/// #include <header.h>
	class item final {
	private:
		header& _owner;
		size_t _index = -1;

	public:
		/// Constructor.
		explicit item(header& owner, size_t index) noexcept
			: _owner{owner}, _index{index} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const item& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_index == other._index;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const item& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::header to which this item belongs.
		[[nodiscard]] const header& owner() const noexcept { return this->_owner; }
		/// Returns the wl::header to which this item belongs.
		[[nodiscard]] header&       owner() noexcept       { return this->_owner; }

		/// Returns the zero-based index of this item.
		[[nodiscard]] size_t index() const noexcept { return this->_index; }

		/// Retrieves the LPARAM associated to the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem
		[[nodiscard]] LPARAM lparam() const
		{
			HDITEMW hdi{};
			hdi.mask = HDI_LPARAM;

			if (Header_GetItem(this->_owner.hwnd(), this->_index, &hdi) == FALSE) {
				throw std::runtime_error("Header_GetItem() failed in " __FUNCTION__ "().");
			}
			return hdi.lParam;
		}

		/// Retrieves the order in which the item appears.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-header_getitem
		[[nodiscard]] size_t order() const
		{
			HDITEMW hdi{};
			hdi.mask = HDI_ORDER;

			if (Header_GetItem(this->_owner.hwnd(), this->_index, &hdi) == FALSE) {
				throw std::runtime_error("Header_GetItem() failed in " __FUNCTION__ "().");
			}
			return hdi.iOrder;
		}

		/// Deletes the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-header_deleteitem
		void remove() const
		{
			if (Header_DeleteItem(this->_owner.hwnd(), this->_index)) {
				throw std::runtime_error("Header_DeleteItem() failed in " __FUNCTION__ "().");
			}
		}

		/// Sets the LPARAM associated to the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setitem
		const item& set_lparam(LPARAM lp) const
		{
			HDITEMW hdi{};
			hdi.mask = HDI_LPARAM;
			hdi.lParam = lp;

			if (Header_SetItem(this->_owner.hwnd(), this->_index, &hdi) == 0) {
				throw std::runtime_error("Header_SetItem() failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Sets the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-header_setitem
		const item& set_text(std::wstring_view text) const
		{
			HDITEM hdi{};
			hdi.mask = HDI_TEXT;
			hdi.pszText = const_cast<wchar_t*>(text.data());

			if (Header_SetItem(this->_owner.hwnd(), this->_index, &hdi) == 0) {
				throw std::runtime_error("Header_SetItem() failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Retrieves the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-header_getitem
		[[nodiscard]] std::wstring text() const
		{
			wchar_t buf[128]{}; // arbitrary length

			HDITEMW hdi{};
			hdi.mask = HDI_TEXT;
			hdi.pszText = buf;
			hdi.cchTextMax = ARRAYSIZE(buf);

			if (Header_GetItem(this->_owner.hwnd(), this->_index, &hdi) == FALSE) {
				throw std::runtime_error("Header_GetItem() failed in " __FUNCTION__ "().");
			}
			return {buf};
		}
	};

private:
	/// Provides access to the items of a wl::header.
	///
	/// You can't create this object, it's created internally by the header.
	///
	/// #include <header.h>
	class item_collection final {
	private:
		friend header;
		header& _owner;

		explicit item_collection(header& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the item at the given index.
		/// @warning Does not perform bound checking.
		[[nodiscard]] item operator[](size_t itemIndex) const noexcept { return item{this->_owner, itemIndex}; }

		/// Adds a new item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-header_insertitem
		item add(std::wstring_view text, UINT width) const
		{
			HDITEMW hdi{};
			hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
			hdi.pszText = const_cast<wchar_t*>(text.data());
			hdi.fmt = HDF_LEFT | HDF_STRING;
			hdi.cxy = width;

			size_t idx = Header_InsertItem(this->_owner.hwnd(), this->count(), &hdi);
			if (idx == -1) {
				throw std::runtime_error("Header_InsertItem() failed in " __FUNCTION__ "().");
			}
			return this->operator[](idx);
		}

		/// Retrieves the number of items.
		[[nodiscard]] size_t count() const
		{
			size_t c = Header_GetItemCount(this->_owner.hwnd());
			if (c == -1) {
				throw std::runtime_error("Header_GetItemCount() failed in " __FUNCTION__ "().");
			}
			return c;
		}
	};

	_wli::base_native_control _base;
	msg::header::wnd_events _events;

public:
	/// Access to the header items.
	item_collection items{*this};

	/// Constructor, explicitly defining the control ID.
	header(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit header(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	header(header&&) = default;

	/// Move assignment operator.
	header& operator=(header&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param hStyles Header control styles. HDS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/button-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	header& create(POINT pos, SIZE size,
		DWORD hStyles = HDS_BUTTONS | HDS_HORZ,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(WC_HEADERW, {}, pos, size, hStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		return *this;
	}

	/// Calls CreateWindowEx(), with a standard height of 24 pixels.
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param hStyles Header control styles. HDS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/button-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	header& create(POINT pos, UINT width,
		DWORD hStyles = HDS_BUTTONS | HDS_HORZ,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		return this->create(pos, {static_cast<LONG>(width), 24},
			hStyles, wStyles, wExStyles);
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::header::wnd_events& on() noexcept { return this->_events; }

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
	const header& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Adjusts width and height to fit the given rectangle.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-header_layout
	const header& adjust_to_fit(const RECT &rc) const
	{
		RECT rcBuf = rc;
		WINDOWPOS wpos{};

		HDLAYOUT hdl{};
		hdl.prc = &rcBuf;
		hdl.pwpos = &wpos;

		if (Header_Layout(this->hwnd(), &hdl) == FALSE) {
			throw std::runtime_error("Header_Layout() failed in " __FUNCTION__ "().");
		}
		
		if (SetWindowPos(this->hwnd(), wpos.hwndInsertAfter,
			wpos.x, wpos.y, wpos.cx, wpos.cy, wpos.flags) == 0)
		{
			throw std::system_error(GetLastError(), std::system_category(),
					"SetWindowPos() failed in " __FUNCTION__ "().");
		}
		return *this;
	}
};

}//namespace wl