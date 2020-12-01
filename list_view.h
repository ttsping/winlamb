/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/interfaces.h"
#include "internal/msg_wm.h"
#include "internal/msg_wnd_events_ctrl.h"
#include "image_list.h"
#include "menu.h"
#include "str.h"

namespace wl {

/// Native list view control. Not to be confused with the simpler wl::list_box.
///
/// #include <list_view.h>
///
/// @note The list view is already subclassed with on_subclass(), default handling these messages:
/// - wl::msg::wnd_events::wm_get_dlg_code()
/// - wl::msg::wnd_events::wm_r_button_down()
///
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/list-view-controls-overview
class list_view final : public i_resizable_control, public i_resource_control {
public:
	/// A single item of a wl::list_view.
	/// It's a simple object that can be cheaply copied.
	///
	/// #include <list_view.h>
	class item final {
	private:
		list_view& _owner;
		size_t _index = -1;

	public:
		/// Constructor.
		explicit item(list_view& owner, size_t index) noexcept
			: _owner{owner}, _index{index} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const item& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_index == other._index;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const item& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::list_view to which this item belongs.
		[[nodiscard]] const list_view& owner() const noexcept { return this->_owner; }
		/// Returns the wl::list_view to which this item belongs.
		[[nodiscard]] list_view&       owner() noexcept       { return this->_owner; }

		/// Returns the zero-based index of this item.
		[[nodiscard]] size_t index() const noexcept { return this->_index; }

		/// Scrolls the list view, if needed, so the item becomes visible.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_ensurevisible
		const item& ensure_visible() const
		{
			if (ListView_EnsureVisible(this->_owner.hwnd(), this->_index, 1) == FALSE) {
				throw std::runtime_error("LVM_ENSUREVISIBLE failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Retrieves the zero-based index of the wl::image_list icon.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem
		[[nodiscard]] int icon_index() const
		{
			LVITEMW lvi{};
			lvi.iItem = static_cast<int>(this->_index);
			lvi.mask = LVIF_IMAGE;

			if (ListView_GetItem(this->_owner.hwnd(), &lvi) == FALSE) {
				throw std::runtime_error("ListView_GetItem() failed in " __FUNCTION__ "().");
			}
			return lvi.iImage;
		}
		
		/// Tells if the item is currently the focused one.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem
		[[nodiscard]] bool is_focused() const { return (_state(LVIS_FOCUSED) & LVIS_FOCUSED) != 0; }

		/// Tells if the item is currently selected.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem
		[[nodiscard]] bool is_selected() const { return (_state(LVIS_SELECTED) & LVIS_SELECTED) != 0; }

		/// Tells if the item is currently visible.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_isitemvisible
		[[nodiscard]] bool is_visible() const noexcept { return ListView_IsItemVisible(this->_owner.hwnd(), this->_index) != 0; }

		/// Retrieves the LPARAM associated to the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem
		[[nodiscard]] LPARAM lparam() const
		{
			LVITEMW lvi{};
			lvi.iItem = static_cast<int>(this->_index);
			lvi.mask = LVIF_PARAM;

			if (ListView_GetItem(this->_owner.hwnd(), &lvi) == FALSE) {
				throw std::runtime_error("ListView_GetItem() failed in " __FUNCTION__ "().");
			}
			return lvi.lParam;
		}

		/// Gets the bounding rectangle for the item.
		/// @return Coordinates relative to list view.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitemrect
		[[nodiscard]] RECT rect(int lvirPortion = LVIR_BOUNDS) const
		{
			RECT rcItem{};
			rcItem.left = lvirPortion;

			if (ListView_GetItemRect(
				this->_owner.hwnd(), this->_index, &rcItem, lvirPortion) == FALSE)
			{
				throw std::runtime_error("ListView_GetItemRect() failed in " __FUNCTION__ "().");
			}
			return rcItem;
		}

		/// Deletes the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_deleteitem
		void remove() const
		{
			if (ListView_DeleteItem(this->_owner.hwnd(), this->_index) == FALSE) {
				throw std::runtime_error("ListView_DeleteItem() failed in " __FUNCTION__ "().");
			}
		}

		/// Sets the item as the focused one.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setitemstate
		const item& set_focused() const noexcept
		{
			ListView_SetItemState(this->_owner.hwnd(), this->_index, LVIS_FOCUSED, LVIS_FOCUSED);
			return *this;
		}

		/// Sets the zero-based index of the wl::image_list icon.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setitem
		const item& set_icon_index(int iconIndex) const
		{
			LVITEMW lvi{};
			lvi.iItem = static_cast<int>(this->_index);
			lvi.mask = LVIF_IMAGE;
			lvi.iImage = iconIndex;

			if (ListView_SetItem(this->_owner.hwnd(), &lvi) == FALSE) {
				throw std::runtime_error("ListView_SetItem() failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Sets the LPARAM associated to the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setitem
		const item& set_lparam(LPARAM lp) const
		{
			LVITEMW lvi{};
			lvi.iItem = static_cast<int>(this->_index);
			lvi.mask = LVIF_PARAM;
			lvi.lParam = lp;

			if (ListView_SetItem(this->_owner.hwnd(), &lvi) == FALSE) {
				throw std::runtime_error("ListView_SetItem() failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Selects or deselects the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-setitemstate
		const item& set_selected(bool isSelected) const
		{
			LVITEMW lvi{};
			lvi.stateMask = LVIS_SELECTED;
			lvi.state = isSelected ? LVIS_SELECTED : 0;

			if (SendMessageW(this->_owner.hwnd(), LVM_SETITEMSTATE,
				this->_index, reinterpret_cast<LPARAM>(&lvi)) == FALSE)
			{
				throw std::runtime_error("LVM_SETITEMSTATE failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Sets the texts at all columns at once.
		/// @param columnTexts Texts of each column.
		const item& set_column_texts(
			std::initializer_list<std::wstring_view> columnTexts) const
		{
			for (size_t i = 0; i < columnTexts.size(); ++i) {
				this->set_subitem_text(i, *(columnTexts.begin() + i));
			}
			return *this;
		}

		/// Sets the text of the item at the given column.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-setitemtext
		const item& set_subitem_text(size_t columnIndex, std::wstring_view text) const
		{
			LVITEMW lvi{};
			lvi.iSubItem = static_cast<int>(columnIndex);
			lvi.pszText = const_cast<wchar_t*>(text.data());

			if (SendMessageW(this->_owner.hwnd(), LVM_SETITEMTEXT,
				this->_index, reinterpret_cast<LPARAM>(&lvi)) == FALSE)
			{
				throw std::runtime_error(
					wl::str::unicode_to_ansi(
						wl::str::format(L"LVM_SETITEMTEXT failed to set text \"%s\" in " __FUNCTION__ "().",
							text)));
			}
			return *this;
		}

		/// Sets the text of the item at the first column.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-setitemtext
		const item& set_text(std::wstring_view text) const { return this->set_subitem_text(0, text); }

		/// Retrieves the text at the given column.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-getitemtext
		[[nodiscard]] std::wstring subitem_text(size_t columnIndex) const
		{
			// http://forums.codeguru.com/showthread.php?351972-Getting-listView-item-text-length
			LVITEMW lvi{};
			lvi.iSubItem = static_cast<int>(columnIndex);

			// Notice that, since strings' size always increase, if the buffer
			// was previously allocated with a value bigger than our 1st step,
			// this will speed up the size checks.

			std::wstring buf(64, L'\0'); // speed-up 1st allocation
			int baseBufLen = 0;
			int charsWrittenWithoutNull = 0;

			do {
				baseBufLen += 64; // buffer increasing step, arbitrary
				buf.resize(baseBufLen);
				lvi.cchTextMax = baseBufLen;
				lvi.pszText = &buf[0];
				charsWrittenWithoutNull = static_cast<int>(
					SendMessageW(this->_owner.hwnd(), LVM_GETITEMTEXT,
						this->_index, reinterpret_cast<LPARAM>(&lvi)) );
			} while (charsWrittenWithoutNull == baseBufLen - 1); // to break, must have at least 1 char gap

			buf.resize( lstrlenW(buf.c_str()) ); // trim nulls
			return buf;
		}

		/// Retrieves the text at the first column.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-getitemtext
		[[nodiscard]] std::wstring text() const { return this->subitem_text(0); }

		/// Updates the item, rearranging the list view.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_update
		const item& update() const
		{
			if (ListView_Update(this->_owner.hwnd(), this->_index) == FALSE) {
				throw std::runtime_error("ListView_Update() failed in " __FUNCTION__ "().");
			}
			return *this;
		}

	private:
		[[nodiscard]] UINT _state(UINT stateMask) const
		{
			LVITEMW lvi{};
			lvi.iItem = static_cast<int>(this->_index);
			lvi.mask = LVIF_STATE;
			lvi.stateMask = stateMask;

			if (ListView_GetItem(this->_owner.hwnd(), &lvi) == FALSE) {
				throw std::runtime_error("ListView_GetItem() failed in " __FUNCTION__ "().");
			}
			return lvi.state;
		}
	};

	/// A single column of a list view.
	/// It's a simple object that can be cheaply copied.
	///
	/// #include <list_view.h>
	class column final {
	private:
		list_view& _owner;
		size_t _index = -1;

	public:
		/// Constructor.
		explicit column(list_view& owner, size_t index) noexcept
			: _owner{owner}, _index{index} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const column& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_index == other._index;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const column& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::list_view to which this column belongs.
		[[nodiscard]] const list_view& owner() const noexcept { return this->_owner; }
		/// Returns the wl::list_view to which this column belongs.
		[[nodiscard]] list_view&       owner() noexcept       { return this->_owner; }

		/// Returns the zero-based index of this column.
		[[nodiscard]] size_t index() const noexcept { return this->_index; }

		/// Retrieves the texts of all items, at this column.
		[[nodiscard]] std::vector<std::wstring> items_texts() const
		{
			std::vector<std::wstring> texts;
			texts.reserve(ListView_GetItemCount(this->_owner.hwnd()));

			int idx = -1;
			for (;;) {
				idx = ListView_GetNextItem(this->_owner.hwnd(), idx, LVNI_ALL);
				if (idx == -1) break;

				item theItem{this->_owner, static_cast<size_t>(idx)};
				texts.emplace_back(theItem.subitem_text(this->_index));
			}

			return texts;
		}

		/// Retrieves the texts of the selected items, at this column.
		[[nodiscard]] std::vector<std::wstring> selected_items_texts() const
		{
			std::vector<std::wstring> texts;
			texts.reserve(ListView_GetSelectedCount(this->_owner.hwnd()));

			int idx = -1;
			for (;;) {
				idx = ListView_GetNextItem(this->_owner.hwnd(), idx, LVNI_SELECTED);
				if (idx == -1) break;

				item theItem{this->_owner, static_cast<size_t>(idx)};
				texts.emplace_back(theItem.subitem_text(this->_index));
			}

			return texts;
		}

		/// Sets the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-setcolumn
		const column& set_text(std::wstring_view text) const
		{
			LVCOLUMNW lvc{};
			lvc.iSubItem = static_cast<int>(this->_index);
			lvc.mask = LVCF_TEXT;
			lvc.pszText = const_cast<wchar_t*>(text.data());

			if (SendMessageW(this->_owner.hwnd(), LVM_SETCOLUMN,
				this->_index, reinterpret_cast<LPARAM>(&lvc)) == FALSE)
			{
				throw std::runtime_error(
					wl::str::unicode_to_ansi(
						wl::str::format(L"LVM_SETCOLUMN failed to set text \"%s\" in " __FUNCTION__ "().",
							text)));
			}
			return *this;
		}

		/// Sets the width in pixels. Will be adjusted to match current system DPI.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-setcolumnwidth
		const column& set_width(UINT width) const
		{
			SIZE colWidth = _wli::control_visuals::multiply_dpi(SIZE{static_cast<LONG>(width), 0});

			if (!SendMessageW(this->_owner.hwnd(), LVM_SETCOLUMNWIDTH, this->_index, colWidth.cx)) {
				throw std::runtime_error("LVM_SETCOLUMNWIDTH failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Expands or shrinks the column to fill the remaning space.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-setcolumnwidth
		const column& set_width_to_fill() const
		{
			size_t numCols = this->_owner.columns.count();
			size_t cxUsed = 0;

			for (size_t i = 0; i < numCols; ++i) {
				if (i != this->_index) { // retrieve cx of each column, but chosen one
					size_t cx = static_cast<size_t>(
						SendMessageW(this->_owner.hwnd(), LVM_GETCOLUMNWIDTH, i, 0) );
					if (cx == 0) {
						throw std::runtime_error("LVM_GETCOLUMNWIDTH failed in " __FUNCTION__ "().");
					}
					cxUsed += cx;
				}
			}

			RECT rc{};
			GetClientRect(this->_owner.hwnd(), &rc); // list view client area
			size_t cxToFill = static_cast<LONG>(
				rc.right
				//- GetSystemMetrics(SM_CXVSCROLL)
				- cxUsed);

			if (SendMessageW(this->_owner.hwnd(), LVM_SETCOLUMNWIDTH, this->_index, cxToFill) == FALSE) {
				throw std::runtime_error("LVM_SETCOLUMNWIDTH failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Retrieves the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-getcolumn
		[[nodiscard]] std::wstring text() const
		{
			wchar_t buf[128]{}; // arbitrary length

			LVCOLUMNW lvc{};
			lvc.iSubItem = static_cast<int>(this->_index);
			lvc.mask = LVCF_TEXT;
			lvc.pszText = buf;
			lvc.cchTextMax = ARRAYSIZE(buf);

			if (SendMessageW(this->_owner.hwnd(), LVM_GETCOLUMN,
				this->_index, reinterpret_cast<LPARAM>(&lvc)) == FALSE)
			{
				throw std::runtime_error("LVM_GETCOLUMN failed in " __FUNCTION__ "().");
			}
			return {buf};
		}

		/// Retrieves the width in pixels.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-getcolumnwidth
		[[nodiscard]] UINT width() const
		{
			size_t cx = SendMessageW(this->_owner.hwnd(), LVM_GETCOLUMNWIDTH, this->_index, 0);
			if (!cx) {
				throw std::runtime_error("LVM_GETCOLUMNWIDTH failed in " __FUNCTION__ "().");
			}
			return static_cast<UINT>(cx);
		}
	};

private:
	/// Provides access to the items of a wl::list_view.
	///
	/// You can't create this object, it's created internally by the list view.
	///
	/// #include <list_view.h>
	class item_collection final {
	private:
		friend list_view;
		list_view& _owner;

		explicit item_collection(list_view& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the item at the given index.
		/// @warning Does not perform bound checking.
		[[nodiscard]] item operator[](size_t itemIndex) const noexcept { return item{this->_owner, itemIndex}; }

		/// Adds a new item.
		/// @param text The text of the item, to be displayed under the first column.
		/// @return The newly added wl::list_view_item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_insertitem
		item add(std::wstring_view text) const { return this->add_with_icon(text, -1); }

		/// Adds a new item, also setting the texts on all columns.
		/// @param columnTexts Texts of each column.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_insertitem
		item add_with_columns(
			std::initializer_list<std::wstring_view> columnTexts) const
		{
			return this->add_with_columns_and_icon(columnTexts, -1);
		}

		/// Adds a new item, also setting the texts on all columns.
		/// @note Before calling this method, call set_image_list() to make the icons available to the list view.
		/// @param columnTexts Texts of each column.
		/// @param iIcon Zero-based index of the icon, from the associated wl::image_list, to be displayed.
		/// @return The newly added wl::list_view_item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_insertitem
		item add_with_columns_and_icon(
			std::initializer_list<std::wstring_view> columnTexts, int iIcon) const
		{
			if (columnTexts.size() == 0) {
				throw std::invalid_argument("No texts given in " __FUNCTION__ "().");
			}
			
			item newItem = this->add_with_icon(*columnTexts.begin(), iIcon);
			for (size_t i = 1; i < columnTexts.size(); ++i) {
				newItem.set_subitem_text(i, *(columnTexts.begin() + i));
			}
			return newItem;
		}

		/// Adds a new item.
		/// @note Before calling this method, call set_image_list() to make the icons available to the list view.
		/// @param text The text of the item, to be displayed under the first column.
		/// @param iIcon Zero-based index of the icon, from the associated wl::image_list, to be displayed.
		/// @return The newly added wl::list_view_item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_insertitem
		item add_with_icon(std::wstring_view text, int iIcon) const
		{
			LVITEMW lvi{};
			lvi.mask = LVIF_TEXT | LVIF_IMAGE;
			lvi.pszText = const_cast<wchar_t*>(text.data());
			lvi.iItem = 0x0fff'ffff; // insert as the last one
			lvi.iImage = iIcon;

			size_t newIdx = ListView_InsertItem(this->_owner.hwnd(), &lvi);
			if (newIdx == -1) {
				throw std::runtime_error(
					wl::str::unicode_to_ansi(
						wl::str::format(L"ListView_InsertItem() failed for \"%s\" in " __FUNCTION__ "().",
							text)));
			}
			return this->operator[](newIdx);
		}

		/// Retrieves all items in the list view at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getnextitem
		[[nodiscard]] std::vector<item> all() const
		{
			std::vector<item> theItems;
			theItems.reserve(this->count());

			int idx = -1;
			for (;;) {
				idx = ListView_GetNextItem(this->_owner.hwnd(), idx, LVNI_ALL);
				if (idx == -1) break;

				theItems.emplace_back(this->operator[](idx));
			}

			return theItems;
		}

		/// Retrieves the number of items.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitemcount
		[[nodiscard]] size_t count() const noexcept { return ListView_GetItemCount(this->_owner.hwnd()); }

		/// Retrieves the item with the given text, case-insensitive, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_finditem
		[[nodiscard]] std::optional<item> find(std::wstring_view text) const noexcept
		{
			LVFINDINFOW lfi{};
			lfi.flags = LVFI_STRING;
			lfi.psz = text.data();

			int idx = ListView_FindItem(this->_owner.hwnd(), -1, &lfi);
			if (idx == -1) {
				return std::nullopt;
			}
			return {this->operator[](idx)};
		}

		/// Retrieves the currently focused item, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getnextitem
		[[nodiscard]] std::optional<item> focused() const noexcept
		{
			int idx = ListView_GetNextItem(this->_owner.hwnd(), -1, LVNI_FOCUSED);
			if (idx == -1) {
				return std::nullopt;
			}
			return {this->operator[](idx)};
		}

		/// Deletes the given items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_deleteitem
		const list_view& remove(const std::vector<item>& items) const
		{
			std::vector<size_t> sortedIdxs;
			sortedIdxs.reserve(items.size());

			for (const item& it : items) {
				if (it.owner().hwnd() != this->_owner.hwnd()) {
					throw std::logic_error("Cannot delete item from another list view with " __FUNCTION__ "().");
				}
				sortedIdxs.emplace_back(it.index());
			}
			std::sort(sortedIdxs.begin(), sortedIdxs.end(), std::greater()); // sort in reverse order

			for (size_t idx : sortedIdxs) {
				if (ListView_DeleteItem(this->_owner.hwnd(), idx) == FALSE) {
					throw std::runtime_error("ListView_DeleteItem() failed in " __FUNCTION__ "().");
				}
			}
			return this->_owner;
		}

		/// Deletes the given items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_deleteitem
		const list_view& remove(std::initializer_list<size_t> indexes) const
		{
			std::vector<size_t> sortedIdxs = indexes;
			std::sort(sortedIdxs.begin(), sortedIdxs.end(), std::greater()); // sort in reverse order

			for (size_t idx : sortedIdxs) {
				if (ListView_DeleteItem(this->_owner.hwnd(), idx) == FALSE) {
					throw std::runtime_error("ListView_DeleteItem() failed in " __FUNCTION__ "().");
				}
			}
			return this->_owner;
		}

		/// Deletes all items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_deleteallitems
		const list_view& remove_all() const
		{
			if (ListView_DeleteAllItems(this->_owner.hwnd()) == FALSE) {
				throw std::runtime_error("ListView_DeleteAllItems() failed in " __FUNCTION__ "().");
			}
			return this->_owner;
		}

		/// Deletes all items currently selected.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getnextitem
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_deleteitem
		const list_view& remove_selected() const noexcept
		{
			int i = -1;
			while ((i = ListView_GetNextItem(this->_owner.hwnd(), -1, LVNI_SELECTED)) != -1) {
				ListView_DeleteItem(this->_owner.hwnd(), i);
			}
			return this->_owner;
		}

		/// Retrieves the selected items, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getnextitem
		[[nodiscard]] std::vector<item> selected() const
		{
			std::vector<item> selItems;
			selItems.reserve(this->selected_count());

			int idx = -1;
			for (;;) {
				idx = ListView_GetNextItem(this->_owner.hwnd(), idx, LVNI_SELECTED);
				if (idx == -1) break;
				selItems.emplace_back(this->_owner, idx);
			}
			return selItems;
		}

		/// Retrieves the number of selected items.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getselectedcount
		[[nodiscard]] size_t selected_count() const noexcept { return ListView_GetSelectedCount(this->_owner.hwnd()); }

		/// Retrieves the indexes of the selected items, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getnextitem
		[[nodiscard]] std::vector<size_t> selected_indexes() const
		{
			std::vector<size_t> indexes;
			indexes.reserve(this->selected_count());

			int idx = -1;
			for (;;) {
				idx = ListView_GetNextItem(this->_owner.hwnd(), idx, LVNI_SELECTED);
				if (idx == -1) break;
				indexes.emplace_back(idx);
			}
			return indexes;
		}

		/// Selects or deselects all items.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setitemstate
		const list_view& set_selected_all(bool isSelected) const noexcept
		{
			ListView_SetItemState(this->_owner.hwnd(), -1,
				isSelected ? LVIS_SELECTED : 0, LVIS_SELECTED);
			return this->_owner;
		}

		/// Selects or deselects the given items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setitemstate
		const list_view& set_selected(
			const std::vector<item>& items, bool isSelected) const
		{
			for (const item& it : items) {
				if (it.owner().hwnd() != this->_owner.hwnd()) {
					throw std::logic_error("Cannot select item from another list view with " __FUNCTION__ "().");
				}
				it.set_selected(isSelected);
			}
			return this->_owner;
		}

		/// Selects or deselects the given items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setitemstate
		const list_view& set_selected(
			std::vector<size_t> indexes, bool isSelected) const noexcept
		{
			for (size_t idx : indexes) {
				ListView_SetItemState(this->_owner.hwnd(), idx,
					isSelected ? LVIS_SELECTED : 0, LVIS_SELECTED);
			}
			return this->_owner;
		}

		/// Selects or deselects the given items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setitemstate
		const list_view& set_selected(
			std::initializer_list<size_t> indexes, bool isSelected) const noexcept
		{
			for (size_t idx : indexes) {
				ListView_SetItemState(this->_owner.hwnd(), idx,
					isSelected ? LVIS_SELECTED : 0, LVIS_SELECTED);
			}
			return this->_owner;
		}
	};

	/// Provides access to the columns of a wl::list_view.
	///
	/// You can't create this object, it's created internally by the list view.
	///
	/// #include <list_view.h>
	class column_collection final {
	private:
		friend list_view;
		list_view& _owner;

		explicit column_collection(list_view& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the column at the given index.
		/// @warning Does not perform bound checking.
		[[nodiscard]] column operator[](size_t columnIndex) const noexcept { return column{this->_owner, columnIndex}; }

		/// Adds a new column.
		/// @param Title of the column.
		/// @param width Initial width. Will be adjusted to match current system DPI.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_insertcolumn
		const list_view& add(std::wstring_view text, UINT width) const
		{
			SIZE colWidth = _wli::control_visuals::multiply_dpi(SIZE{static_cast<LONG>(width), 0});

			LVCOLUMNW lvc{};
			lvc.mask = LVCF_TEXT | LVCF_WIDTH;
			lvc.pszText = const_cast<wchar_t*>(text.data());
			lvc.cx = colWidth.cx;

			if (ListView_InsertColumn(this->_owner.hwnd(), 0xffff, &lvc) == -1) {
				throw std::runtime_error(
					wl::str::unicode_to_ansi(
						wl::str::format(L"ListView_InsertColumn() failed for \"%s\" in " __FUNCTION__ "().",
							text)));
			}
			return this->_owner;
		}

		/// Retrieves the number of columns.
		[[nodiscard]] size_t count() const
		{
			size_t count = SendMessageW(this->_header(), HDM_GETITEMCOUNT, 0, 0);
			if (count == -1) {
				throw std::runtime_error("HDM_GETITEMCOUNT failed in " __FUNCTION__ "().");
			}
			return count;
		}

		/// Enables or disable the column resizing.
		const list_view& enable_resizing(bool isEnabled) const
		{
			// https://www.vbforums.com/showthread.php?673570-Listview-disable-column-width-modification
			EnableWindow(this->_header(), isEnabled);
			return this->_owner;
		}

	private:
		HWND _header() const
		{
			HWND hHeader = reinterpret_cast<HWND>(
				SendMessageW(this->_owner.hwnd(), LVM_GETHEADER, 0, 0) );
			if (hHeader == nullptr) {
				throw std::runtime_error("LVM_GETHEADER failed in " __FUNCTION__ "().");
			}
			return hHeader;
		}
	};

	_wli::base_native_control _base;
	msg::list_view::wnd_events _events;
	menu _contextMenu;

public:
	/// Access to the list view columns.
	column_collection columns{*this};

	/// Access to the list view items.
	item_collection items{*this};

	/// Constructor, explicitly defining the control ID.
	list_view(i_parent_window* parent, WORD ctrlId)
		: _base{*parent, ctrlId}, _events{_base}
	{
		this->_add_default_subclass_handlers();
	}

	/// Constructor, with an auto-generated control ID.
	explicit list_view(i_parent_window* parent)
		: _base{*parent}, _events{_base}
	{
		this->_add_default_subclass_handlers();
	}

	/// Move constructor
	list_view(list_view&&) = default;

	/// Move assignment operator.
	list_view& operator=(list_view&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param lvStyles List view control styles. LVS_ constants. For safety, LVS_SHAREIMAGELISTS will always be added.
	/// @param lvExStyles Extended list view control styles. LVS_EX_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/list-view-window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/extended-list-view-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	list_view& create(POINT pos, SIZE size,
		DWORD lvStyles = LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS,
		DWORD lvExStyles = 0,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = WS_EX_CLIENTEDGE)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(WC_LISTVIEWW, {}, pos, size,
			LVS_SHAREIMAGELISTS | lvStyles | wStyles, wExStyles);

		if (lvExStyles != 0) {
			this->set_extended_style(true, lvExStyles);
		}
		return *this;
	}

private:
	void create_in_dialog() override
	{
		this->_base.create_window();
		if ((GetWindowLongPtrW(this->hwnd(), GWL_STYLE) & LVS_SHAREIMAGELISTS) == 0) {
			throw std::logic_error("The list view doesn't have LVS_SHAREIMAGELISTS style, you must add it.");
		}
	}

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::list_view::wnd_events& on() noexcept { return this->_events; }

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
	const list_view& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Returns the context menu attached to this list view.
	[[nodiscard]] const menu& context_menu() const noexcept { return this->_contextMenu; }

	/// Retrieves the LVS_EX_ styles.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getextendedlistviewstyle
	[[nodiscard]] DWORD extended_style() const noexcept
	{
		return ListView_GetExtendedListViewStyle(this->hwnd());
	}

	/// Sends LVM_HITTEST to determine the item at specified position, if any.
	/// Position coordinates must be relative to list view.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_hittestex
	[[nodiscard]] LVHITTESTINFO hit_test(POINT pos) const noexcept
	{
		LVHITTESTINFO lvht{};
		lvht.pt = pos;

		ListView_HitTestEx(this->hwnd(), &lvht);
		return lvht;
	}

	/// Sets a popup menu to be used as the context menu.
	/// The list view doesn't own the menu, and it must remain valid to be used.
	list_view& set_context_menu(const menu& popupMenu) noexcept { this->_contextMenu = popupMenu; return *this; }

	/// Sets or unsets list view extended styles
	/// @param isSet Tells if the flags will be set or unset.
	/// @param lvExStyles Extended list view control styles. LVS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvm-setextendedlistviewstyle
	const list_view& set_extended_style(bool isSet, DWORD lvExStyles) const noexcept
	{
		SendMessageW(this->hwnd(), LVM_SETEXTENDEDLISTVIEWSTYLE,
			lvExStyles, isSet ? lvExStyles : 0);
		return *this;
	}

	/// Sets the associated wl::image_list.
	/// The image list object is shared, and must remain valid.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_setimagelist
	list_view& set_image_list(
		const image_list& imageList, DWORD lvsilType = LVSIL_SMALL) noexcept
	{
		// This method is non-const because it's usually called during object creation,
		// which chains many non-const methods.
		ListView_SetImageList(this->hwnd(), imageList.himagelist(), lvsilType);
		return *this;
	}

	/// Sends a WM_SETREDRAW.
	/// Value false prevents the control from being redrawn.
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-setredraw
	const list_view& set_redraw(bool doRedraw) const noexcept
	{
		SendMessageW(this->hwnd(), WM_SETREDRAW,
			static_cast<WPARAM>(static_cast<BOOL>(doRedraw)), 0);
		return *this;
	}

private:
	void _add_default_subclass_handlers()
	{
		this->on_subclass().wm_get_dlg_code([this](msg::wm_get_dlg_code p) noexcept -> WORD
		{
			if (!p.is_query() && p.vkey_code() == 'A' && p.has_ctrl()) { // Ctrl+A to select all items
				this->items.set_selected_all(true);
				return DLGC_WANTCHARS;
			} else if (!p.is_query() && p.vkey_code() == VK_RETURN) { // send Enter key to parent
#pragma warning (disable: 26454) // https://stackoverflow.com/a/51142504/6923555
				NMLVKEYDOWN nmlvkd = {
					{
						this->hwnd(),
						static_cast<WORD>(this->ctrl_id()),
						LVN_KEYDOWN // this triggers warning 26454: arithmetic overflow
					},
					VK_RETURN, 0
				};
#pragma warning (default: 26454)
				SendMessageW(GetAncestor(this->hwnd(), GA_PARENT),
					WM_NOTIFY, reinterpret_cast<WPARAM>(this->hwnd()),
					reinterpret_cast<LPARAM>(&nmlvkd));
				return DLGC_WANTALLKEYS;
			} else if (!p.is_query() && p.vkey_code() == VK_APPS) { // context menu keyboard key
				this->_show_context_menu(false, p.has_ctrl(), p.has_shift());
			}
			return static_cast<WORD>(DefSubclassProc(this->hwnd(), WM_GETDLGCODE, p.wparam, p.lparam));
		});

		this->on_subclass().wm_r_button_down([this](msg::wm_r_button_down p) noexcept
		{
			this->_show_context_menu(true, p.has_ctrl(), p.has_shift());
		});
	}

	int _show_context_menu(bool followCursor, bool hasCtrl, bool hasShift) noexcept
	{
		if (this->_contextMenu.hmenu() == nullptr) return -1; // no context menu assigned

		POINT coords{};
		int itemBelowCursor = -1;
		if (followCursor) { // usually fired with a right-click
			LVHITTESTINFO lvhti{};
			GetCursorPos(&lvhti.pt); // relative to screen
			ScreenToClient(this->hwnd(), &lvhti.pt); // now relative to list view
			ListView_HitTest(this->hwnd(), &lvhti); // item below cursor, if any
			coords = lvhti.pt;
			itemBelowCursor = lvhti.iItem; // -1 if none
			if (itemBelowCursor != -1) { // an item was right-clicked
				if (!hasCtrl && !hasShift) {
					if ((ListView_GetItemState(this->hwnd(), itemBelowCursor, LVIS_SELECTED) & LVIS_SELECTED) == 0) {
						// If right-clicked item isn't currently selected, unselect all and select just it.
						ListView_SetItemState(this->hwnd(), -1, 0, LVIS_SELECTED);
						ListView_SetItemState(this->hwnd(), itemBelowCursor, LVIS_SELECTED, LVIS_SELECTED);
					}
					ListView_SetItemState(this->hwnd(), itemBelowCursor, LVIS_FOCUSED, LVIS_FOCUSED); // focus clicked
				}
			} else if (!hasCtrl && !hasShift) {
				ListView_SetItemState(this->hwnd(), -1, 0, LVIS_SELECTED); // unselect all
			}
			SetFocus(this->hwnd()); // because a right-click won't set the focus by default
		} else { // usually fired with the context menu keyboard key
			int itemFocused = ListView_GetNextItem(this->hwnd(), -1, LVNI_FOCUSED);
			if (itemFocused != -1 && ListView_IsItemVisible(this->hwnd(), itemFocused)) { // item focused and visible
				RECT rcItem{};
				ListView_GetItemRect(this->hwnd(), itemFocused, &rcItem, LVIR_BOUNDS); // relative to list view
				coords = {rcItem.left + 16, rcItem.top + (rcItem.bottom - rcItem.top) / 2};
			} else { // no focused and visible item
				coords = {6, 10};
			}
		}

		// The popup menu is created with hDlg as parent, so the menu messages go to it.
		// The lvhti coordinates are relative to list view, and will be mapped into screen-relative.
		this->_contextMenu.show_at_point(GetParent(this->hwnd()), coords, this->hwnd());
		return itemBelowCursor; // -1 if none
	}
};

}//namespace wl