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
#include "str.h"

namespace wl {

/// Simply holds a menu handle (HMENU), not freeing it in destructor.
///
/// It's a simple object that can be cheaply copied.
///
/// #include <menu.h>
/// @see @ref ex11
/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/about-menus
class menu {
protected:
	HMENU _hMenu = nullptr;

public:
	virtual ~menu() { }

	/// Default constructor.
	menu() = default;

	/// Copy constructor.
	menu(const menu&) = default;

	/// Constructor. Simply stores the HMENU handle.
	explicit menu(HMENU hMenu) noexcept : _hMenu{hMenu} { }

	/// Copy assignment operator.
	menu& operator=(const menu&) = default;

	/// Equality operator.
	[[nodiscard]] bool operator==(const menu& other) const noexcept { return this->_hMenu == other._hMenu; }
	/// Inequality operator.
	[[nodiscard]] bool operator!=(const menu& other) const noexcept { return !this->operator==(other); }

	/// Returns the underlying HMENU handle.
	[[nodiscard]] HMENU hmenu() const noexcept { return this->_hMenu; }

	/// Appends a new item.
	/// @param cmdId The command ID that will be sent in a WM_COMMAND message when the menu item is clicked.
	/// @param text The text of the menu item.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-appendmenuw
	const menu& append_item(WORD cmdId, std::wstring_view text) const
	{
		if (AppendMenuW(this->_hMenu, MF_STRING, cmdId, text.data()) == FALSE) {
			throw std::system_error(GetLastError(), std::system_category(),
				"AppendMenu() failed in " __FUNCTION__ "().");
		}
		return *this;
	}

	/// Appends a new separator.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-appendmenuw
	const menu& append_separator() const
	{
		if (AppendMenuW(this->_hMenu, MF_SEPARATOR, 0, nullptr) == FALSE) {
			throw std::system_error(GetLastError(), std::system_category(),
				"AppendMenu() failed in " __FUNCTION__ "().");
		}
		return *this;
	}

	/// Calls CreateMenuPopup() and appends the new submenu.
	/// @param text Text of the submenu entry.
	/// @return The newly appended submenu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-appendmenuw
	menu append_submenu(std::wstring_view text) const
	{
		HMENU pop = _create_submenu();

		if (AppendMenuW(this->_hMenu, MF_STRING | MF_POPUP,
			reinterpret_cast<UINT_PTR>(pop), text.data()) == FALSE)
		{
			throw std::system_error(GetLastError(), std::system_category(),
				"AppendMenu() failed in " __FUNCTION__ "().");
		}

		return menu{pop};
	}

	/// Inserts a new item right before the item with the given command ID.
	/// @param cmdIdBefore Command ID which identifies an entry, the new item will be inserted before it.
	/// @param newCmdId Command ID of the new item being inserted.
	/// @param Text of the new entry.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-insertmenuw
	const menu& insert_item_before_cmd(
		WORD cmdIdBefore, WORD newCmdId, std::wstring_view text) const
	{
		return this->_insert_item_before(cmdIdBefore, newCmdId, text, false);
	}
	/// Inserts a new item right before the item at the given position.
	/// @param posBefore Zero-based index of an entry, the new item will be inserted before it.
	/// @param newCmdId Command ID of the new item being inserted.
	/// @param Text of the new entry.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-insertmenuw
	const menu& insert_item_before_pos(
		size_t posBefore, WORD newCmdId, std::wstring_view text) const
	{
		return this->_insert_item_before(
			static_cast<UINT>(posBefore), newCmdId, text, true);
	}

	/// Calls CreateMenuPopup() and inserts the new submenu right before the item with the given command ID.
	/// @param cmdIdBefore Command ID which identifies an entry, the new submenu will be inserted before it.
	/// @param text Text of the submenu entry.
	/// @return The newly inserted submenu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-insertmenuw
	menu insert_submenu_before_cmd(WORD cmdIdBefore, std::wstring_view text) const
	{
		return this->_insert_submenu_before(cmdIdBefore, text, false);
	}
	/// Calls CreateMenuPopup() and inserts the new submenu right before the item at the given position.
	/// @param posBefore Zero-based index of an entry, the new item will be inserted before it.
	/// @param text Text of the submenu entry.
	/// @return The newly inserted submenu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-insertmenuw
	menu insert_submenu_before_pos(size_t posBefore, std::wstring_view text) const
	{
		return this->_insert_submenu_before(
			static_cast<UINT>(posBefore), text, true);
	}

	/// Retrieves the command ID of the menu item at the given position, if any.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmenuitemid
	[[nodiscard]] std::optional<WORD> cmd_of_pos(size_t pos) const
	{
		WORD cmdId = GetMenuItemID(this->_hMenu, static_cast<WORD>(pos));
		if (cmdId == -1) {
			return std::nullopt;
		}
		return {cmdId};
	}

	/// Retrieves the sub menu at the given position, if any.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getsubmenu
	[[nodiscard]] std::optional<menu> sub_menu(size_t pos) const
	{
		HMENU hSub = GetSubMenu(this->_hMenu, static_cast<int>(pos));
		if (hSub == nullptr) {
			return std::nullopt;
		}
		return {menu{hSub}};
	}

	/// Retrieves the number of items.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmenuitemcount
	[[nodiscard]] size_t item_count() const
	{
		int count = GetMenuItemCount(this->_hMenu);
		if (count == -1) {
			throw std::system_error(GetLastError(), std::system_category(),
				"GetMenuItemCount() failed in " __FUNCTION__ "().");
		}
		return count;
	}

	/// Deletes all items at once.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-deletemenu
	const menu& delete_all_items() const
	{
		for (size_t i = this->item_count(); i-- > 0; ) {
			this->delete_by_pos(i);
		}
		return *this;
	}

	/// Deletes the item with the given command ID.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-deletemenu
	const menu& delete_by_cmd(WORD cmdId) const { return this->_delete(cmdId, false); }
	/// Deletes the item at the given position.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-deletemenu
	const menu& delete_by_pos(size_t pos) const { return this->_delete(static_cast<UINT>(pos), true); }

	/// Enables or disables the item with the given command ID.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enablemenuitem
	const menu& enable_by_cmd(WORD cmdId, bool isEnabled) const { return this->_enable(cmdId, isEnabled, false); }
	/// @return A reference to the same original menu.
	/// Enables or disables the item at the given position.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enablemenuitem
	const menu& enable_by_pos(size_t pos, bool isEnabled) const { return this->_enable(static_cast<UINT>(pos), isEnabled, true); }

	/// Enables or disables the items with the given command IDs at once.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enablemenuitem
	const menu& enable_by_cmd(std::initializer_list<WORD> cmdIds, bool isEnabled) const
	{
		for (WORD cmdId : cmdIds) {
			this->enable_by_cmd(cmdId, isEnabled);
		}
		return *this;
	}
	/// Enables or disables the items at the given positions at once.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enablemenuitem
	const menu& enable_by_pos(std::initializer_list<size_t> poss, bool isEnabled) const
	{
		for (size_t pos : poss) {
			this->enable_by_pos(pos, isEnabled);
		}
		return *this;
	}

	/// Sets the text of the item with the given command ID.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setmenuiteminfow
	const menu& set_text_by_cmd(WORD cmdId, std::wstring_view text) const { return this->_set_text(cmdId, text, false); }
	/// Sets the text of the item at the given position.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setmenuiteminfow
	const menu& set_text_by_pos(size_t pos, std::wstring_view text) const { return this->_set_text(static_cast<UINT>(pos), text, true); }

	/// Retrieves the text of the item with the given command ID.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmenuiteminfow
	[[nodiscard]] std::wstring text_by_cmd(WORD cmdId) const { return this->_text(cmdId, false); }
	/// Retrieves the text of the item at the given position.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmenuiteminfow
	[[nodiscard]] std::wstring text_by_pos(size_t pos) const { return this->_text(static_cast<UINT>(pos), true); }

	/// Shows the floating menu anchored at the given coordinates with TrackPopupMenu().
	/// If hCoordsRelativeTo is null, coordinates must be relative to hParent.
	/// This function will block until the menu disappears.
	/// @return A reference to the same original menu.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-trackpopupmenu
	const menu& show_at_point(
		HWND hParent, POINT pt, HWND hWndCoordsRelativeTo) const
	{
		POINT ptParent = pt; // receives coordinates relative to hParent
		if (ClientToScreen(hWndCoordsRelativeTo ?
			hWndCoordsRelativeTo : hParent, &ptParent) == FALSE) // to screen coordinates
		{
			throw std::runtime_error("ClientToScreen() failed in " __FUNCTION__ "().");
		}

		SetForegroundWindow(hParent);

		if (TrackPopupMenu(this->_hMenu, TPM_LEFTBUTTON,
			ptParent.x, ptParent.y, 0, hParent, nullptr) == FALSE) // owned by hParent, so messages go to it
		{
			throw std::system_error(GetLastError(), std::system_category(),
				"TrackPopupMenu() failed in " __FUNCTION__ "().");
		}

		PostMessageW(hParent, WM_NULL, 0, 0); // http://msdn.microsoft.com/en-us/library/ms648002%28VS.85%29.aspx
		return *this;
	}

private:
	const menu& _delete(UINT cmdOrPos, bool byPos) const
	{
		if (DeleteMenu(this->_hMenu, cmdOrPos, byPos ? MF_BYPOSITION : MF_BYCOMMAND) == FALSE) {
			throw std::system_error(GetLastError(), std::system_category(),
				str::unicode_to_ansi(
					str::format(L"DeleteMenu() %d failed in " __FUNCTION__ "().", cmdOrPos) ));
		}
		return *this;
	}

	const menu& _enable(UINT cmdOrPos, bool isEnabled, bool byPos) const
	{
		UINT flags = (isEnabled ? MF_ENABLED : MF_GRAYED)
			| (byPos ? MF_BYPOSITION : MF_BYCOMMAND);

		if (EnableMenuItem(this->_hMenu, cmdOrPos, flags) == -1) {
			throw std::logic_error(
				str::unicode_to_ansi(
					str::format(L"The menu item %d doesn't exist in " __FUNCTION__ "().",
						cmdOrPos)));
		}
		return *this;
	}

	const menu& _insert_item_before(
		UINT cmdOrPosBefore, WORD newCmdId, std::wstring_view text, bool byPos) const
	{
		UINT flag = byPos ? MF_BYPOSITION : MF_BYCOMMAND;

		if (InsertMenuW(this->_hMenu, cmdOrPosBefore,
			flag | MF_STRING, newCmdId, text.data() ) == FALSE)
		{
			throw std::system_error(GetLastError(), std::system_category(),
				str::unicode_to_ansi(
					str::format(L"InsertMenu() failed for \"%s\" in " __FUNCTION__ "().",
						text)));
		}
		return *this;
	}

	menu _insert_submenu_before(
		UINT cmdOrPosBefore, std::wstring_view text, bool byPos) const
	{
		UINT flag = byPos ? MF_BYPOSITION : MF_BYCOMMAND;
		HMENU pop = _create_submenu();

		if (InsertMenuW(this->_hMenu, cmdOrPosBefore,
			flag | MF_POPUP, reinterpret_cast<UINT_PTR>(pop), text.data() ) == FALSE)
		{
			throw std::system_error(GetLastError(), std::system_category(),
				str::unicode_to_ansi(
					str::format(L"InsertMenu() failed for \"%s\" in " __FUNCTION__ "().",
						text)));
		}

		return menu{pop};
	}

	const menu& _set_text(UINT cmdOrPos, std::wstring_view text, bool byPos) const
	{
		MENUITEMINFOW mii{};
		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = const_cast<wchar_t*>(text.data());

		if (SetMenuItemInfoW(this->_hMenu, cmdOrPos, byPos, &mii) == FALSE) {
			throw std::system_error(GetLastError(), std::system_category(),
				str::unicode_to_ansi(
					str::format(L"InsertMenu() failed for \"%s\" in " __FUNCTION__ "().",
						text)));
		}
		return *this;
	}

	[[nodiscard]] const std::wstring _text(UINT cmdOrPos, bool byPos) const
	{
		MENUITEMINFOW mii{};
		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STRING;

		if (GetMenuItemInfoW(this->_hMenu, cmdOrPos, byPos, &mii) == FALSE) { // retrieve length
			throw std::system_error(GetLastError(), std::system_category(),
				str::unicode_to_ansi(
					str::format(L"GetMenuItemInfo() failed to retrieve text length from %d in " __FUNCTION__ "().",
						cmdOrPos)));
		}
		++mii.cch; // add room for terminating null

		std::wstring buf(mii.cch, L'\0');
		mii.dwTypeData = &buf[0];
		if (GetMenuItemInfoW(this->_hMenu, cmdOrPos, byPos, &mii) == FALSE) {
			throw std::system_error(GetLastError(), std::system_category(),
				str::unicode_to_ansi(
					str::format(L"GetMenuItemInfo() failed to retrieve text from %d in " __FUNCTION__ "().",
						cmdOrPos)));
		}
		return buf;
	}

	[[nodiscard]] static HMENU _create_submenu()
	{
		HMENU pop = CreatePopupMenu();
		if (pop == nullptr) {
			throw std::system_error(GetLastError(), std::system_category(),
				"CreatePopupMenu() failed in " __FUNCTION__ "().");
		}
		return pop;
	}
};

/// Manages a horizontal main window menu.
/// Calls CreateMenu() in constructor.
/// Supposed to be attached to a window, which will destroy it automatically.
///
/// #include <menu.h>
/// @see @ref ex11
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createmenu
class menu_main final : public menu {
public:
	/// Default constructor. Calls CreateMenu().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createmenu
	menu_main() : menu{CreateMenu()}
	{
		if (this->_hMenu == nullptr) {
			throw std::system_error(GetLastError(), std::system_category(),
				"CreateMenu() failed in " __FUNCTION__ "().");
		}
	}

	/// Move constructor.
	menu_main(menu_main&& other) noexcept { this->operator=(std::move(other)); }

	/// Move assignment operator.
	menu_main& operator=(menu_main&& other) noexcept
	{
		DestroyMenu(this->_hMenu);
		this->_hMenu = nullptr;
		std::swap(this->_hMenu, other._hMenu);
		return *this;
	}
};

/// Manages a popup menu.
/// Calls CreatePopupMenu() in constructor, DestroyMenu() in destructor.
///
/// #include <menu.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createpopupmenu
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroymenu
class menu_popup final : public menu {
public:
	/// Destructor. Calls destroy().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroymenu
	~menu_popup() { this->destroy(); }

	/// Default constructor. Calls CreatePopupMenu().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createpopupmenu
	menu_popup() : menu{CreatePopupMenu()}
	{
		if (this->_hMenu == nullptr) {
			throw std::system_error(GetLastError(), std::system_category(),
				"CreatePopupMenu() failed in " __FUNCTION__ "().");
		}
	}

	/// Move constructor.
	menu_popup(menu_popup&& other) noexcept { this->operator=(std::move(other)); }

	/// Move assignment operator.
	menu_popup& operator=(menu_popup&& other) noexcept
	{
		this->destroy();
		std::swap(this->_hMenu, other._hMenu);
		return *this;
	}

	/// Calls DestroyMenu().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroymenu
	void destroy() noexcept
	{
		if (this->_hMenu != nullptr) {
			DestroyMenu(this->_hMenu);
			this->_hMenu = nullptr;
		}
	}
};

/// Manages a menu loaded from the resource.
/// Calls LoadMenu() in constructor.
/// Loaded resources are automatically destroyed by the system.
///
/// #include <menu.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadmenuw
class menu_resource final : public menu {
public:
	/// Default constructor.
	menu_resource() = default;

	/// Constructor. Calls load().
	explicit menu_resource(WORD menuId) { this->load(menuId); }

	/// Move constructor.
	menu_resource(menu_resource&& other) noexcept { this->operator=(std::move(other)); }

	/// Move assignment operator.
	menu_resource& operator=(menu_resource&& other) noexcept
	{
		this->_hMenu = nullptr;
		std::swap(this->_hMenu, other._hMenu);
		return *this;
	}

	/// Calls LoadMenu().
	/// @param menuId ID of the menu resource to be loaded, usually declared in a .rc file.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadmenuw
	menu_resource& load(WORD menuId)
	{
		this->_hMenu = LoadMenuW(GetModuleHandle(nullptr), MAKEINTRESOURCEW(menuId));
		if (this->_hMenu == nullptr) {
			throw std::system_error(GetLastError(), std::system_category(),
				"LoadMenu() failed in " __FUNCTION__ "().");
		}
		return *this;
	}
};

}//namespace wl