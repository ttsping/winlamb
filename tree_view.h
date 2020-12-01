/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <Windows.h>
#include <CommCtrl.h>
#include "internal/base_native_control.h"
#include "internal/control_visuals.h"
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"
#include "image_list.h"

namespace wl {

/// Native tree view control.
///
/// #include <tree_view.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tree-view-controls
class tree_view final : public i_resizable_control, public i_resource_control {
public:
	/// A single item of the wl::tree_view.
	/// It's a simple object that can be cheaply copied.
	///
	/// #include <tree_view.h>
	class item final {
	private:
		tree_view& _owner;
		HTREEITEM _hTreeItem = nullptr;

	public:
		/// Constructor.
		explicit item(tree_view& owner, HTREEITEM hTreeItem) noexcept
			: _owner{owner}, _hTreeItem{hTreeItem} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const item& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_hTreeItem == other._hTreeItem;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const item& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::tree_view to which this item belongs.
		[[nodiscard]] const tree_view& owner() const noexcept { return this->_owner; }
		/// Returns the wl::tree_view to which this item belongs.
		[[nodiscard]] tree_view&       owner() noexcept       { return this->_owner; }

		/// Returns the underlying HTREEITEM handle.
		[[nodiscard]] HTREEITEM htreeitem() const noexcept { return this->_hTreeItem; }

		/// Adds a new child node.
		/// @return The newly added child node.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_insertitem
		item add_child(std::wstring_view text) const
		{
			return add_child_with_icon(text, -1);
		}

		/// Adds a new child node.
		/// You must attach an image list to see the icon.
		/// @return The newly added child node.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_insertitem
		item add_child_with_icon(std::wstring_view text, int iIcon) const
		{
			TVINSERTSTRUCTW tvi{};
			tvi.hParent = this->_hTreeItem;
			tvi.hInsertAfter = TVI_LAST;
			tvi.itemex.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvi.itemex.pszText = const_cast<wchar_t*>(text.data());
			tvi.itemex.iImage = iIcon;
			tvi.itemex.iSelectedImage = iIcon;

			HTREEITEM newItem = TreeView_InsertItem(this->_owner.hwnd(), &tvi);
			if (newItem == nullptr) {
				throw std::runtime_error(
					wl::str::unicode_to_ansi(
						wl::str::format(L"TreeView_InsertItem() failed for \"%s\" in " __FUNCTION__ "().",
							text)));
			}
			return item{this->_owner, newItem};
		}

		/// Ensures that a tree-view item is visible, expanding the
		/// parent item or scrolling the tree-view control, if needed.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_ensurevisible
		const item& ensure_visible() const noexcept
		{
			TreeView_EnsureVisible(this->_owner.hwnd(), this->_hTreeItem);
			return *this;
		}

		/// Retrieves the first child node, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getchild
		[[nodiscard]] std::optional<item> first_child() const noexcept
		{
			HTREEITEM hti = TreeView_GetChild(this->_owner.hwnd(), this->_hTreeItem);
			if (hti != nullptr) {
				return {item{this->_owner, hti}};
			}
			return std::nullopt;
		}

		/// Tells if the node is currently expanded.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getitemstate
		[[nodiscard]] bool is_expanded() const noexcept
		{
			return (
				TreeView_GetItemState(this->_owner.hwnd(),
					this->_hTreeItem, TVIS_EXPANDED) & TVIS_EXPANDED)
				!= 0;
		}

		/// Tells if the node is a root node.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getparent
		[[nodiscard]] bool is_root() const noexcept
		{
			return TreeView_GetParent(this->_owner.hwnd(), this->_hTreeItem) == nullptr;
		}

		/// Tells if the item is the currently selected one.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getselection
		[[nodiscard]] bool is_selected() const noexcept
		{
			return this->_hTreeItem == TreeView_GetSelection(this->_owner.hwnd());
		}

		/// Retrieves the LPARAM associated to the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getitem
		[[nodiscard]] LPARAM lparam() const
		{
			TVITEMEXW tvi{};
			tvi.hItem = this->_hTreeItem;
			tvi.mask = TVIF_PARAM;

			if (TreeView_GetItem(this->_owner.hwnd(), &tvi) == FALSE) {
				throw std::runtime_error("TreeView_GetItem() failed in " __FUNCTION__ "().");
			}
			return tvi.lParam;
		}

		/// Retrieves the next sibling node, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getnextsibling
		[[nodiscard]] std::optional<item> next_sibling() const noexcept
		{
			HTREEITEM hti = TreeView_GetNextSibling(this->_owner.hwnd(), this->_hTreeItem);
			if (hti != nullptr) {
				return {item{this->_owner, hti}};
			}
			return std::nullopt;
		}

		/// Retrieves the next visible node, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getnextvisible
		[[nodiscard]] std::optional<item> next_visible() const noexcept
		{
			HTREEITEM hti = TreeView_GetNextVisible(this->_owner.hwnd(), this->_hTreeItem);
			if (hti != nullptr) {
				return {item{this->_owner, hti}};
			}
			return std::nullopt;
		}

		/// Retrieves the parent node, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getparent
		[[nodiscard]] std::optional<item> parent() const noexcept
		{
			HTREEITEM hti = TreeView_GetParent(this->_owner.hwnd(), this->_hTreeItem);
			if (hti != nullptr) {
				return {item{this->_owner, hti}};
			}
			return std::nullopt;
		}

		/// Deletes the node.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_deleteitem
		void remove() const
		{
			if (TreeView_DeleteItem(this->_owner.hwnd(), this->_hTreeItem) == FALSE) {
				throw std::runtime_error("TreeView_DeleteItem() failed in " __FUNCTION__ "().");
			}
		}

		/// Expands or collapses the node.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_expand
		const item& set_expanded(bool isExpanded) const noexcept
		{
			TreeView_Expand(this->_owner.hwnd(), this->_hTreeItem,
				isExpanded ? TVE_EXPAND : TVE_COLLAPSE);
			return *this;
		}

		/// Sets the LPARAM associated to the item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_setitem
		const item& set_lparam(LPARAM lp) const
		{
			TVITEMEXW tvi{};
			tvi.hItem = this->_hTreeItem;
			tvi.mask = TVIF_PARAM;
			tvi.lParam = lp;

			if (TreeView_SetItem(this->_owner.hwnd(), &tvi) == FALSE) {
				throw std::runtime_error("TreeView_SetItem() failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Sets the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_setitem
		const item& set_text(std::wstring_view text) const
		{
			TVITEMEX tvi{};
			tvi.hItem = this->_hTreeItem;
			tvi.mask = TVIF_TEXT;
			tvi.pszText = const_cast<wchar_t*>(text.data());

			if (TreeView_SetItem(this->_owner.hwnd(), &tvi) == FALSE) {
				throw std::runtime_error(
					wl::str::unicode_to_ansi(
						wl::str::format(L"TreeView_SetItem() failed for \"%s\" in " __FUNCTION__ "().",
							text)));
			}
			return *this;
		}

		/// Retrieves the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getitem
		[[nodiscard]] std::wstring text() const
		{
			wchar_t buf[MAX_PATH]{}; // arbitrary length

			TVITEMEX tvi{};
			tvi.hItem = this->_hTreeItem;
			tvi.mask = TVIF_TEXT;
			tvi.cchTextMax = ARRAYSIZE(buf);
			tvi.pszText = buf;

			if (TreeView_GetItem(this->_owner.hwnd(), &tvi) == FALSE) {
				throw std::runtime_error("TreeView_GetItem() failed in " __FUNCTION__ "().");
			}
			return buf;
		}

		/// Toggles the node, expanded or collapsed.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_expand
		const item& toggle_expanded() const noexcept
		{
			TreeView_Expand(this->_owner.hwnd(), this->_hTreeItem, TVE_TOGGLE);
			return *this;
		}
	};

private:
	/// Provides access to the items of a wl::tree_view.
	///
	/// You can't create this object, it's created internally by the tree view.
	///
	/// #include <tree_view.h>
	class item_collection final {
	private:
		friend tree_view;
		tree_view& _owner;

		explicit item_collection(tree_view& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the item with the given HTREEITEM handle.
		///
		/// @param hTreeItem The HTREEITEM handle of the item to be retrieved.
		/// @warning This method simply returns the object, without validating the
		/// HTREEITEM handle.
		[[nodiscard]] item operator[](HTREEITEM hTreeItem) const noexcept { return item{this->_owner, hTreeItem}; }

		/// Adds a new root node, returning it.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_insertitem
		item add_root(std::wstring_view text) const { return add_root_with_icon(text, -1); }

		/// Adds a new root node, returning it.
		/// You must attach an image list to see the icon.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_insertitem
		item add_root_with_icon(std::wstring_view text, int iIcon) const
		{
			TVINSERTSTRUCTW tvi{};
			tvi.hParent = TVI_ROOT;
			tvi.hInsertAfter = TVI_LAST;
			tvi.itemex.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvi.itemex.pszText = const_cast<wchar_t*>(text.data());
			tvi.itemex.iImage = iIcon;
			tvi.itemex.iSelectedImage = iIcon;

			HTREEITEM newItem = TreeView_InsertItem(this->_owner.hwnd(), &tvi);
			if (newItem == nullptr) {
				throw std::runtime_error(
					wl::str::unicode_to_ansi(
						wl::str::format(L"TreeView_InsertItem failed for \"%s\" in item_collection::add_root_with_icon().",
							text)));
			}
			return this->operator[](newItem);
		}

		/// Returns the total number of nodes.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getcount
		[[nodiscard]] size_t count() const noexcept { return TreeView_GetCount(this->_owner.hwnd()); }

		/// Retrieves the first root node, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getroot
		[[nodiscard]] std::optional<item> first_root() const noexcept
		{
			HTREEITEM hti = TreeView_GetRoot(this->_owner.hwnd());
			if (hti != nullptr) {
				return {this->operator[](hti)};
			}
			return std::nullopt;
		}

		/// Retrieves the first visible node, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getfirstvisible
		[[nodiscard]] std::optional<item> first_visible() const noexcept
		{
			HTREEITEM hti = TreeView_GetFirstVisible(this->_owner.hwnd());
			if (hti != nullptr) {
				return {this->operator[](hti)};
			}
			return std::nullopt;
		}

		/// Retrieves the last visible node, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getlastvisible
		[[nodiscard]] std::optional<item> last_visible() const noexcept
		{
			HTREEITEM hti = TreeView_GetLastVisible(this->_owner.hwnd());
			if (hti != nullptr) {
				return this->operator[](hti);
			}
			return std::nullopt;
		}

		/// Deletes all nodes at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_deleteallitems
		const tree_view& remove_all() const
		{
			if (TreeView_DeleteAllItems(this->_owner.hwnd()) == FALSE) {
				throw std::runtime_error("TreeView_DeleteAllItems() failed in item_collection::remove_all().");
			}
			return this->_owner;
		}

		/// Retrieves all root nodes.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getroot
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getnextsibling
		[[nodiscard]] std::vector<item> roots() const
		{
			std::vector<item> nodes;

			HTREEITEM hti = TreeView_GetRoot(this->_owner.hwnd());
			while (hti != nullptr) {
				nodes.emplace_back(this->_owner, hti);
				hti = TreeView_GetNextSibling(this->_owner.hwnd(), hti);
			}
			return nodes;
		}

		/// Retrieves the selected node, if any.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getselection
		[[nodiscard]] std::optional<item> selected() const noexcept
		{
			HTREEITEM hti = TreeView_GetSelection(this->_owner.hwnd());
			if (hti != nullptr) {
				return {this->operator[](hti)};
			}
			return std::nullopt;
		}

		/// Retrieves the number of visible nodes.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getvisiblecount
		[[nodiscard]] size_t visible_count() const noexcept
		{
			return TreeView_GetVisibleCount(this->_owner.hwnd());
		}
	};

	_wli::base_native_control _base;
	msg::tree_view::wnd_events _events;

public:
	/// Access to the tree view items.
	item_collection items{*this};

	/// Constructor, explicitly defining the control ID.
	tree_view(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit tree_view(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	tree_view(tree_view&&) = default;

	/// Move assignment operator.
	tree_view& operator=(tree_view&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param tvStyles Tree view control styles. TVS_ constants.
	/// @param tvExStyles Extended tree view control styles. TVS_EX_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tree-view-control-window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tree-view-control-window-extended-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	tree_view& create(POINT pos, SIZE size,
		DWORD tvStyles = TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS,
		DWORD tvExStyles = 0,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = WS_EX_CLIENTEDGE)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(WC_TREEVIEWW, {}, pos, size, tvStyles | wStyles, wExStyles);

		if (tvExStyles != 0) {
			this->set_extended_style(true, tvExStyles);
		}
		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::tree_view::wnd_events& on() noexcept { return this->_events; }

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
	const tree_view& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Retrieves the TVS_EX_ styles.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_getextendedstyle
	[[nodiscard]] size_t extended_style() const noexcept
	{
		return TreeView_GetExtendedStyle(this->hwnd());
	}

	/// Sets or unsets tree view extended styles
	/// @param isSet Tells if the flags will be set or unset.
	/// @param lvExStyles Extended tree view control styles. TVS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_setextendedstyle
	const tree_view& set_extended_style(
		bool isSet, DWORD tvExStyles) const noexcept
	{
		TreeView_SetExtendedStyle(this->hwnd(), tvExStyles, isSet ? tvExStyles : 0);
		return *this;
	}

	/// Sets the associated wl::image_list.
	/// The image list object is shared, and must remain valid.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-treeview_setimagelist
	tree_view& set_image_list(
		const wl::image_list& imgList, DWORD tvsilType = TVSIL_NORMAL) noexcept
	{
		// This method is non-const because it's usually called during object creation,
		// which chains many non-const methods.
		TreeView_SetImageList(this->hwnd(), imgList.himagelist(), tvsilType);
		return *this;
	}
};

}//namespace wl