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
#include "internal/base_parent_subclass.h"
#include "internal/control_visuals.h"
#include "internal/gdi_obj.h"
#include "internal/interfaces.h"
#include "internal/msg_wm.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native tab control.
///
/// #include <tab.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tab-controls
class tab final : public i_resizable_control, public i_resource_control {
public:
	/// Determines if the tab control will render the selected child control
	/// automatically.
	enum class render {
		/// The tab control will automatically render the selected child control.
		/// This is done by subclassing the parent window.
		AUTO,
		/// The selected child control will be rendered only when you manually
		/// call items.render_selected(), probably during parent's TCN_SELCHANGE
		/// processing.
		MANUAL
	};

	/// A single individual tab of a wl::tab control.
	/// It's a simple object that can be cheaply copied.
	///
	/// #include <tab.h>
	class item final {
	private:
		tab& _owner;
		size_t _index = -1;

	public:
		/// Constructor.
		explicit item(tab& owner, size_t index) noexcept
			: _owner{owner}, _index{index} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const item& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_index == other._index;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const item& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::tab to which this individual tab belongs.
		[[nodiscard]] const tab& owner() const noexcept { return this->_owner; }
		/// Returns the wl::tab to which this individual tab belongs.
		[[nodiscard]] tab&       owner() noexcept       { return this->_owner; }

		/// Returns the zero-based index of this individual tab.
		[[nodiscard]] size_t index() const noexcept { return this->_index; }

		/// Retrieves the handle to the associated child control, if any.
		[[nodiscard]] HWND child_control() const
		{
			TCITEMW tci{};
			tci.mask = TCIF_PARAM;

			if (TabCtrl_GetItem(this->_owner.hwnd(), this->_index, &tci) == FALSE) {
				throw std::runtime_error("TabCtrl_GetItem() failed in " __FUNCTION__ "().");
			}
			return reinterpret_cast<HWND>(tci.lParam);
		}

		/// Selects this individual tab.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_setcursel
		const item& set_selected() const noexcept
		{
			TabCtrl_SetCurSel(this->_owner.hwnd(), this->_index);
			this->_owner.tabs.render_selected();
			return *this;
		}

		/// Sets the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_setitem
		const item& set_text(std::wstring_view text) const
		{
			TCITEMW tci{};
			tci.mask = TCIF_TEXT;
			tci.pszText = const_cast<wchar_t*>(text.data());

			if (TabCtrl_SetItem(this->_owner.hwnd(), this->_index, &tci) == FALSE) {
				throw std::runtime_error("TabCtrl_SetItem() failed in " __FUNCTION__ "().");
			}
			return *this;
		}

		/// Retrieves the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_getitem
		[[nodiscard]] std::wstring text() const
		{
			wchar_t buf[64]{}; // arbitrary length

			TCITEMW tci{};
			tci.mask = TCIF_TEXT;
			tci.pszText = buf;
			tci.cchTextMax = ARRAYSIZE(buf);

			if (TabCtrl_GetItem(this->_owner.hwnd(), this->_index, &tci) == FALSE) {
				throw std::runtime_error("TabCtrl_GetItem() failed in " __FUNCTION__ "().");
			}
			return {buf};
		}
	};

private:
	/// Provides access to the individual tabs of a wl::tab. You can't create
	/// this object, it's created internally by the tab control.
	/// 
	/// #include <tab.h>
	class item_collection final {
	private:
		friend tab;
		tab& _owner;

		explicit item_collection(tab& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the individual tab at the given index.
		/// @warning Does not perform bound checking.
		[[nodiscard]] item operator[](size_t tabIndex) const noexcept { return item{this->_owner, tabIndex}; }

		/// Adds a new individual tab, along with an associated child control.
		/// @note The associated child control must be already created.
		const tab& add(std::wstring_view text, const i_resizable_control& child) const
		{
			this->_add(text, child.hwnd());

			if (this->_owner._renderAction == render::AUTO
				&& this->selected_index() == this->count() - 1)
			{
				this->render_selected();
			}

			return this->_owner;
		}

		/// Adds a new individual tab, without an associated child window.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_insertitem
		const tab& add_bare(std::wstring_view text) const { return this->_add(text, nullptr); }

		/// Adds many individual tabs at once, without associated child windows.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_insertitem
		const tab& add_bare(std::initializer_list<std::wstring_view> texts) const
		{
			for (std::wstring_view text : texts) {
				this->add_bare(text);
			}
			return this->_owner;
		}

		/// Retrives the number of individual tabs.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_getitemcount
		[[nodiscard]] size_t count() const noexcept { return TabCtrl_GetItemCount(this->_owner.hwnd()); }

		/// Renders the currently selected child control.
		///
		/// If you are using wl::tab::render::AUTO, there's no need to call this
		/// method.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_adjustrect
		const tab& render_selected() const
		{
			HWND hChild = this->selected().child_control();
			if (hChild != nullptr) {
				HWND hParent = GetParent(this->_owner.hwnd());
				RECT rc{};
				GetWindowRect(this->_owner.hwnd(), &rc);
				ScreenToClient(hParent, reinterpret_cast<POINT*>(&rc.left));
				ScreenToClient(hParent, reinterpret_cast<POINT*>(&rc.right));
				TabCtrl_AdjustRect(this->_owner.hwnd(), FALSE, &rc);
				SetWindowPos(hChild, HWND_TOP, rc.left, rc.top,
					rc.right - rc.left, rc.bottom - rc.top, 0);
				SetFocus(hChild);
			}
			return this->_owner;
		}

		/// Retrieves the currently selected individual tab.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_getcursel
		[[nodiscard]] item selected() const noexcept { return this->operator[](this->selected_index()); }

		/// Retrieves the index of the currently selected individual tab.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_getcursel
		[[nodiscard]] size_t selected_index() const noexcept { return TabCtrl_GetCurSel(this->_owner.hwnd()); }

	private:
		const tab& _add(std::wstring_view text, HWND hChild) const
		{
			TCITEMW tci{};
			tci.mask = TCIF_TEXT | TCIF_PARAM;
			tci.pszText = const_cast<wchar_t*>(text.data());
			tci.lParam = reinterpret_cast<LPARAM>(hChild);

			size_t newIdx = TabCtrl_InsertItem(this->_owner.hwnd(), this->count(), &tci);
			if (newIdx == -1) {
				throw std::runtime_error(
					wl::str::unicode_to_ansi(
						wl::str::format(L"TabCtrl_InsertItem() failed for \"%s\" in " __FUNCTION__ "().",
							text)));
			}
			return this->_owner;
		}
	};

	_wli::base_native_control _base;
	msg::tab::wnd_events _events;
	render _renderAction;
	_wli::base_parent_subclass<WM_NOTIFY, msg::wm_notify> _parentNotify;

public:
	/// Access to the individual tabs.
	item_collection tabs{*this};

	/// Constructor, explicitly defining the control ID.
	tab(i_parent_window* parent, WORD ctrlId, render renderAction = render::AUTO) noexcept
		: _base{*parent, ctrlId}, _events{_base}, _renderAction{renderAction} { }

	/// Constructor, with an auto-generated control ID.
	explicit tab(i_parent_window* parent, render renderAction = render::AUTO) noexcept
		: _base{*parent}, _events{_base}, _renderAction{renderAction} { }

	/// Move constructor.
	tab(tab&&) = default;

	/// Move assignment operator.
	tab& operator=(tab&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param text The button text.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system DPI.
	/// @param tcStyles Tab control styles. TCS_ constants.
	/// @param tcExStyles Extended tab control styles. TCS_EX_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/button-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	tab& create(POINT pos, SIZE size,
		DWORD tcStyles = 0,
		DWORD tcExStyles = 0,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | WS_CLIPSIBLINGS,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(WC_TABCONTROLW, {}, pos, size, tcStyles | wStyles, wExStyles);
		_wli::globalUiFont.set_on_control(*this);

		if (tcExStyles != 0) {
			this->set_extended_style(true, tcExStyles);
		}

		this->_subclass_parent();
		return *this;
	}

private:
	void create_in_dialog() override
	{
		this->_base.create_window();
		this->_subclass_parent();
	}

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::tab::wnd_events& on() noexcept { return this->_events; }

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
	const tab& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Retrieves the TCS_EX_ styles.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-tabctrl_getextendedstyle
	[[nodiscard]] DWORD extended_style() const noexcept
	{
		return TabCtrl_GetExtendedStyle(this->hwnd());
	}

	/// Sets or unsets tab control extended styles
	/// @param isSet Tells if the flags will be set or unset.
	/// @param tcExStyles Extended list view control styles. TCS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tcm-setextendedstyle
	const tab& set_extended_style(bool isSet, DWORD tcExStyles) const noexcept
	{
		SendMessageW(this->hwnd(), TCM_SETEXTENDEDSTYLE,
			tcExStyles, isSet ? tcExStyles : 0);
		return *this;
	}

private:
	void _subclass_parent()
	{
		if (this->_renderAction == render::AUTO) {
			this->_parentNotify.subclass(this->_base.parent().hwnd(), [this](msg::wm_notify p) {
				if (p.nmhdr().idFrom == this->ctrl_id() && p.nmhdr().code == TCN_SELCHANGE) {
					this->tabs.render_selected();
				}
			});
		}
	}
};

}//namespace wl