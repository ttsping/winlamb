/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/gdi_obj.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native list box control. Not to be confused with the more complex wl::list_view.
///
///
/// #include <list_box.h>
/// @see @ref ex04
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/list-boxes
class list_box final : public i_resizable_control, public i_resource_control {
public:
	/// A single item of a wl::list_box.
	/// It's a simple object that can be cheaply copied.
	///
	/// #include <list_box.h>
	class item final {
	private:
		list_box& _owner;
		size_t _index = -1;

	public:
		/// Constructor.
		explicit item(list_box& owner, size_t index) noexcept
			: _owner{owner}, _index{index} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const item& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_index == other._index;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const item& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::list_box to which this item belongs.
		[[nodiscard]] const list_box& owner() const noexcept { return this->_owner; }
		/// Returns the wl::list_box to which this item belongs.
		[[nodiscard]] list_box&       owner() noexcept       { return this->_owner; }

		/// Returns the zero-based index of this item.
		[[nodiscard]] size_t index() const noexcept { return this->_index; }

		/// Tells if the item is currently selected.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-getsel
		[[nodiscard]] bool is_selected() const
		{
			size_t r = SendMessage(this->_owner.hwnd(), LB_GETSEL, this->_index, 0);
			if (r == LB_ERR) {
				throw std::runtime_error("LB_GETSEL failed in " __FUNCTION__ "().");
			}
			return r != 0;
		}

		/// Deletes the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-deletestring
		void remove() const noexcept
		{
			SendMessageW(this->_owner.hwnd(), LB_DELETESTRING, this->_index, 0);
		}

		/// Selects or deselects a single item, if the item exists.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-selitemrange
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-setcursel
		const item& set_selected(bool isSelected) const
		{
			if (this->_owner._isMultiSel) {
				SendMessageW(this->_owner.hwnd(), LB_SELITEMRANGE,
					isSelected, MAKELPARAM(this->_index, this->_index));
			} else {
				SendMessageW(this->_owner.hwnd(), LB_SETCURSEL,
					isSelected ? this->_index : -1, 0);
			}
			return *this;
		}

		/// Retrieves the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-gettext
		[[nodiscard]] std::wstring text() const
		{
			size_t len = SendMessageW(this->_owner.hwnd(), LB_GETTEXTLEN, this->_index, 0);
			if (!len || len == LB_ERR) {
				throw std::runtime_error("LB_GETTEXTLEN failed in " __FUNCTION__ "().");
			}

			std::wstring buf(len + 1, L'\0');
			if (SendMessageW(this->_owner.hwnd(), LB_GETTEXT,
				this->_index, reinterpret_cast<LPARAM>(&buf[0]) ) == LB_ERR)
			{
				throw std::runtime_error("LB_GETTEXT failed in " __FUNCTION__ "().");
			}
			buf.resize(len);
			return buf;
		}
	};

private:
	/// Provides access to the items of a wl::list_box.
	///
	/// You can't create this object, it's created internally by the list box.
	///
	/// #include <list_box.h>
	class item_collection final {
	private:
		friend list_box;
		list_box& _owner;

		explicit item_collection(list_box& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the item at the given index.
		/// @warning Does not perform bound checking.
		[[nodiscard]] item operator[](size_t itemIndex) const noexcept { return item{this->_owner, itemIndex}; }

		/// Adds a new item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-addstring
		const list_box& add(std::wstring_view text) const
		{
			LRESULT ret = SendMessageW(this->_owner.hwnd(), LB_ADDSTRING,
				0, reinterpret_cast<LPARAM>(text.data()) );

			if (ret == LB_ERRSPACE) {
				throw std::runtime_error("LB_ADDSTRING failed: Insufficient space to store the new string in " __FUNCTION__ "().");
			} else if (ret == LB_ERR) {
				throw std::runtime_error("LB_ADDSTRING failed in " __FUNCTION__ "().");
			}
			return this->_owner;
		}

		/// Adds many items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-addstring
		const list_box& add(std::initializer_list<std::wstring_view> texts) const
		{
			for (std::wstring_view text : texts) {
				this->add(text);
			}
			return this->_owner;
		}

		/// Retrieves the number of items.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-getcount
		[[nodiscard]] size_t count() const noexcept { return SendMessageW(this->_owner.hwnd(), LB_GETCOUNT, 0, 0); }

		/// Deletes all items.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-resetcontent
		const list_box& remove_all() const noexcept
		{
			SendMessageW(this->_owner.hwnd(), LB_RESETCONTENT, 0, 0);
			return this->_owner;
		}

		/// Retrieves the selected items, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-getselitems
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-getcursel
		[[nodiscard]] std::vector<item> selected() const
		{
			std::vector<item> selItems;

			if (this->_owner._isMultiSel) {
				size_t selCount = this->selected_count();
				if (selCount > 0) {
					std::vector<int> selIndexes(this->selected_count(), 0);
					SendMessageW(this->_owner.hwnd(), LB_GETSELITEMS,
						selIndexes.size(), reinterpret_cast<LPARAM>(&selIndexes[0]));

					selItems.reserve(selIndexes.size());
					for (int idx : selIndexes) {
						selItems.emplace_back(this->_owner, idx);
					}
				}

			} else {
				size_t idx = SendMessageW(this->_owner.hwnd(), LB_GETCURSEL, 0, 0);
				if (idx != CB_ERR) {
					selItems.emplace_back(this->_owner, idx);
				}
			}

			return selItems;
		}

		/// Retrieves the number of selected items.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lb-getselcount
		[[nodiscard]] size_t selected_count() const noexcept
		{
			if (this->_owner._isMultiSel) {
				return SendMessageW(this->_owner.hwnd(), LB_GETSELCOUNT, 0, 0);
			}

			return SendMessageW(
				this->_owner.hwnd(), LB_GETCURSEL, 0, 0) == LB_ERR ? 0 : 1;
		}
	};

	_wli::base_native_control _base;
	msg::list_box::wnd_events _events;
	bool _isMultiSel = false;

public:
	/// Access to the list box items.
	item_collection items{*this};

	/// Constructor, explicitly defining the control ID.
	list_box(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit list_box(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	list_box(list_box&&) = default;

	/// Move assignment operator.
	list_box& operator=(list_box&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param lbStyles List box control styles. LBS_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/list-box-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	list_box& create(POINT pos, SIZE size,
		DWORD lbStyles = LBS_NOTIFY,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = WS_EX_CLIENTEDGE)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(WC_LISTBOXW, {}, pos, size, lbStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);

		this->_isMultiSel = (lbStyles & LBS_MULTIPLESEL) != 0;
		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::list_box::wnd_events& on() noexcept { return this->_events; }

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
	const list_box& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }
};

}//namespace wl