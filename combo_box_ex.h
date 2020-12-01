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
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"
#include "image_list.h"

namespace wl {

/// Native ComboBoxEx control, a combo box which can display icons from a
/// wl::image_list. Not to be confused with the simpler wl::combo_box.
///
/// #include <combo_box_ex.h>
/// @see @ref ex10
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/comboboxex-controls
class combo_box_ex final : public i_resizable_control, public i_resource_control {
public:
	/// A single item of a wl::combo_box_ex.
	/// It's a simple object that can be cheaply copied.
	///
	/// #include <combo_box_ex.h>
	class item final {
	private:
		combo_box_ex& _owner;
		size_t _index = -1;

	public:
		/// Constructor.
		explicit item(combo_box_ex& owner, size_t index) noexcept
			: _owner{owner}, _index{index} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const item& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_index == other._index;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const item& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::combo_box_ex to which this item belongs.
		[[nodiscard]] const combo_box_ex& owner() const noexcept { return this->_owner; }
		/// Returns the wl::combo_box_ex to which this item belongs.
		[[nodiscard]] combo_box_ex&       owner() noexcept       { return this->_owner; }

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
		const combo_box_ex& set_selected() const noexcept
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
	/// Provides access to the items of a wl::combo_box_ex.
	///
	/// You can't create this object, it's created internally by the ComboBoxEx.
	///
	/// #include <combo_box_ex.h>
	class item_collection final {
	private:
		friend combo_box_ex;
		combo_box_ex& _owner;

		explicit item_collection(combo_box_ex& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the item at the given index.
		/// @warning Does not perform bound checking.
		[[nodiscard]] item operator[](size_t itemIndex) const noexcept { return item{this->_owner, itemIndex}; }

		/// Adds a new item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbem-insertitem
		const combo_box_ex& add(std::wstring_view text) const
		{
			COMBOBOXEXITEMW cbxi{};
			cbxi.mask = CBEIF_TEXT;
			cbxi.iItem = -1; // insert as the last one
			cbxi.pszText = const_cast<wchar_t*>(text.data());

			if (SendMessageW(this->_owner.hwnd(), CBEM_INSERTITEM,
				0, reinterpret_cast<LPARAM>(&cbxi) ) == -1)
			{
				throw std::runtime_error("CBEM_INSERTITEM failed in " __FUNCTION__ "().");
			}
			return this->_owner;
		}

		/// Adds many items at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbem-insertitem
		const combo_box_ex& add(std::initializer_list<std::wstring_view> texts) const
		{
			for (std::wstring_view text : texts) {
				this->add(text);
			}
			return this->_owner;
		}

		/// Adds a new item.
		/// @note Before calling this method, call set_image_list() to make the icons available to the ComboBoxEx.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbem-insertitem
		const combo_box_ex& add_with_icon(std::wstring_view text, int iIcon) const
		{
			COMBOBOXEXITEMW cbxi{};
			cbxi.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
			cbxi.iItem = -1; // insert as the last one
			cbxi.pszText = const_cast<wchar_t*>(text.data());
			cbxi.iImage = iIcon;
			cbxi.iSelectedImage= iIcon;

			if (SendMessageW(this->_owner.hwnd(), CBEM_INSERTITEM,
				0, reinterpret_cast<LPARAM>(&cbxi) ) == -1)
			{
				throw std::runtime_error("CBEM_INSERTITEM failed in " __FUNCTION__ "().");
			}
			return this->_owner;
		}

		/// Adds many items at once.
		/// @note Before calling this method, call set_image_list() to make the icons available to the ComboBoxEx.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbem-insertitem
		const combo_box_ex& add_with_icon(
			std::initializer_list<std::pair<std::wstring_view, int>> textsAndIcons) const
		{
			for (const std::pair<std::wstring_view, int>& textAndIcon : textsAndIcons) {
				this->add_with_icon(textAndIcon.first, textAndIcon.second);
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
		const combo_box_ex& remove_all() const noexcept
		{
			SendMessageW(this->_owner.hwnd(), CB_RESETCONTENT, 0, 0);
			return this->_owner;
		}

		/// Clears the selected item.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cb-setcursel
		const combo_box_ex& select_none() const noexcept
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
	msg::combo_box_ex::wnd_events _events;

public:
	/// Access to the ComboBoxEx items.
	item_collection items{*this};

	/// Constructor, explicitly defining the control ID.
	combo_box_ex(i_parent_window* parent, WORD ctrlId) noexcept
		: _base{*parent, ctrlId}, _events{_base} { }

	/// Constructor, with an auto-generated control ID.
	explicit combo_box_ex(i_parent_window* parent) noexcept
		: _base{*parent}, _events{_base} { }

	/// Move constructor.
	combo_box_ex(combo_box_ex&&) = default;

	/// Move assignment operator.
	combo_box_ex& operator=(combo_box_ex&&) = default;

	/// Calls CreateWindowEx().
	/// @note This method is better suited when the control belongs to an
	/// ordinary window (not a dialog), thus being called during parent's
	/// WM_CREATE.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param width Width of the control. Will be adjusted to match current system DPI.
	/// @param cbStyles Combo box control styles. CBS_ constants.
	/// @param cbExStyles Extended combo box control styles. CBES_EX_ constants.
	/// @param wStyles Window styles. WS_ constants.
	/// @param wExStyles Extended window styles. WS_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/combo-box-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
	combo_box_ex& create(POINT pos, UINT width,
		DWORD cbStyles = CBS_DROPDOWNLIST,
		DWORD cbExStyles = 0,
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		DWORD wExStyles = 0)
	{
		pos = _wli::control_visuals::multiply_dpi(pos);

		SIZE size = {static_cast<LONG>(width), 204}; // empirically found for 10 items; combo + open list
		size = _wli::control_visuals::multiply_dpi(size);

		this->_base.create_window(WC_COMBOBOXEXW, {}, pos, size, cbStyles | wStyles, wExStyles);

		if (cbExStyles != 0) {
			this->set_extended_style(true, cbExStyles);
		}
		return *this;
	}

private:
	void create_in_dialog() override { this->_base.create_window(); }

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::combo_box_ex::wnd_events& on() noexcept { return this->_events; }

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
	const combo_box_ex& enable(bool isEnabled) const noexcept { EnableWindow(this->hwnd(), isEnabled); return *this; }

	/// Retrieves the CBES_EX_ styles.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbem-getextendedstyle
	[[nodiscard]] DWORD extended_style() const noexcept
	{
		return static_cast<DWORD>(SendMessageW(this->hwnd(), CBEM_GETEXTENDEDSTYLE, 0, 0));
	}

	/// Sets or unsets ComboBoxEx extended styles
	/// @param isSet Tells if the flags will be set or unset.
	/// @param lvExStyles Extended list view control styles. CBES_EX_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbem-setextendedstyle
	const combo_box_ex& set_extended_style(bool isSet, DWORD cbExStyles) const noexcept
	{
		SendMessageW(this->hwnd(), CBEM_SETEXTENDEDSTYLE,
			cbExStyles, isSet ? cbExStyles : 0);
		return *this;
	}

	/// Sets the associated wl::image_list.
	/// The image list object is shared, and must remain valid.
	combo_box_ex& set_image_list(const image_list& imageList) noexcept
	{
		// This method is non-const because it's usually called during object creation,
		// which chains many non-const methods.
		SendMessageW(this->hwnd(), CBEM_SETIMAGELIST,
			0, reinterpret_cast<LPARAM>(imageList.himagelist()));
		return *this;
	}
};

}//namespace wl