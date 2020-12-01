/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <stdexcept>
#include <string_view>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/gdi_obj.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native combo box (dropdown) control. Not to be confused with the more
/// complex wl::combo_box_ex.
///
/// #include <combo_box.h>
/// @see @ref ex10
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/about-combo-boxes
class combo_box final : public i_resizable_control, public i_resource_control {
public:
	/// A single item of a wl::combo_box.
	/// It's a simple object that can be cheaply copied.
	///
	/// #include <combo_box.h>
	class item final {
	private:
		combo_box& _owner;
		size_t _index = -1;

	public:
		/// Constructor.
		explicit item(combo_box& owner, size_t index) noexcept
			: _owner{owner}, _index{index} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const item& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_index == other._index;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const item& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::combo_box to which this item belongs.
		[[nodiscard]] const combo_box& owner() const noexcept { return this->_owner; }
		/// Returns the wl::combo_box to which this item belongs.
		[[nodiscard]] combo_box&       owner() noexcept       { return this->_owner; }

		/// Returns the zero-based index of this item.
		[[nodiscard]] size_t index() const noexcept { return this->_index; }

		/// Tells if the item is currently selected.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-getcursel
		[[nodiscard]] bool is_selected() const noexcept
		{
			return this->_index == SendMessageW(this->_owner.hwnd(), CB_GETCURSEL, 0, 0);
		}

		/// Deletes the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-deletestring
		void remove() const noexcept
		{
			SendMessageW(this->_owner.hwnd(), CB_DELETESTRING, this->_index, 0);
		}

		/// Selects the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-setcursel
		const combo_box& set_selected() const noexcept
		{
			SendMessageW(this->_owner.hwnd(), CB_SETCURSEL, this->_index, 0);
			return this->_owner;
		}

		/// Retrieves the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-getlbtext
		[[nodiscard]] std::wstring text() const
		{
			size_t len = SendMessageW(this->_owner.hwnd(), CB_GETLBTEXTLEN, this->_index, 0);
			if (len == 0 || len == CB_ERR) {
				throw std::runtime_error("CB_GETTEXT failed in " __FUNCTION__ "().");
			}

			std::wstring buf(len + 1, L'\0');
			if (SendMessageW(this->_owner.hwnd(), CB_GETLBTEXT,
				this->_index, reinterpret_cast<LPARAM>(&buf[0]) ) == CB_ERR)
			{
				throw std::runtime_error("CB_GETTEXT failed in " __FUNCTION__ "().");
			}
			buf.resize(len);
			return buf;
		}
	};

private:
	/// Provides access to the items of a wl::combo_box.
	///
	/// You can't create this object, it's created internally by the combo box.
	///
	/// #include <combo_box.h>
	class item_collection final {
	private:
		friend combo_box;
		combo_box& _owner;

		explicit item_collection(combo_box& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the item at the given index.
		/// @warning Does not perform bound checking.
		[[nodiscard]] item operator[](size_t itemIndex) const noexcept { return item{this->_owner, itemIndex}; }

		/// Adds a new item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-addstring
		const combo_box& add(std::wstring_view text) const
		{
			LRESULT ret = SendMessageW(this->_owner.hwnd(), CB_ADDSTRING,
				0, reinterpret_cast<LPARAM>(text.data()) );

			if (ret == CB_ERRSPACE) {
				throw std::runtime_error("CB_ADDSTRING failed: insufficient space to store the new string, in " __FUNCTION__ "().");
			} else if (ret == CB_ERR) {
				throw std::runtime_error("CB_ADDSTRING failed in " __FUNCTION__ "().");
			}
			return this->_owner;
		}

		/// Adds many items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-addstring
		const combo_box& add(std::initializer_list<std::wstring_view> texts) const
		{
			for (std::wstring_view text : texts) {
				this->add(text);
			}
			return this->_owner;
		}

		/// Retrieves the number of items.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-getcount
		[[nodiscard]] size_t count() const noexcept
		{
			return SendMessageW(this->_owner.hwnd(), CB_GETCOUNT, 0, 0);
		}

		/// Deletes all items.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-resetcontent
		const combo_box& remove_all() const noexcept
		{
			SendMessageW(this->_owner.hwnd(), CB_RESETCONTENT, 0, 0);
			return this->_owner;
		}

		/// Clears the selected item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-setcursel
		const combo_box& select_none() const noexcept
		{
			SendMessageW(this->_owner.hwnd(), CB_SETCURSEL, -1, 0);
			return this->_owner;
		}

		/// Retrieves the selected item, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-getcursel
		[[nodiscard]] std::optional<item> selected() const noexcept
		{
			size_t idx = SendMessageW(this->_owner.hwnd(), CB_GETCURSEL, 0, 0);
			if (idx == CB_ERR) {
				return std::nullopt;
			}
			return item{this->_owner, idx};
		}
	};

	_wli::base_native_control _base;
	msg::combo_box::wnd_events _events;

public:
	/// Access to the combo box items.
	item_collection items{*this};

	/// Constructor, explicitly defining the control ID.
	combo_box(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit combo_box(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	combo_box(combo_box&&) = default;

	/// Move assignment operator.
	combo_box& operator=(combo_box&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param width Width of the control. Will be adjusted to match current system DPI.
	/// @param cbStyles Combo box control styles. CBS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/combo-box-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	combo_box& create(POINT pos, UINT width,
		DWORD cbStyles = CBS_DROPDOWNLIST,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);

		SIZE size = {static_cast<LONG>(width), 0};
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(L"COMBOBOX", {}, pos, size, cbStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);
		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::combo_box::wnd_events& on() noexcept { return this->_events; }

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
	const combo_box& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Shows or hide the list box.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-showdropdown
	const combo_box& show_dropdown(bool isVisible) const noexcept
	{
		SendMessageW(this->hwnd(), CB_SHOWDROPDOWN, isVisible, 0);
		return *this;
	}
};

}//namespace wl