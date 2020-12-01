/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <string_view>
#include <vector>
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/base_parent_subclass.h"
#include "internal/interfaces.h"
#include "internal/msg_wm.h"
#include "internal/msg_wnd_events_ctrl.h"
#include "icon.h"
#include "str.h"

namespace wl {

/// Native status bar control.
///
/// #include <status_bar.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/status-bars
class status_bar final : i_control {
public:
	/// Determines if the status bar will be resized automatically when the
	/// parent is resized.
	enum class adjust {
		/// The status bar will be automatically resized when the parend is
		/// resized. This is done by subclassing the parent window.
		AUTO,
		/// The status bar will be resized only when you manually call
		/// resize_to_fit_parent(), preferably during parent's WM_SIZE processing.
		MANUAL
	};

	/// A single part of a wl::status_bar
	/// It's a simple object that can be cheaply copied.
	/// 
	/// #include <status_bar.h>
	class part final {
	private:
		status_bar& _owner;
		size_t _index;

	public:
		/// Constructor.
		explicit part(status_bar& owner, size_t index) noexcept
			: _owner{owner}, _index{index} { }

		/// Equality operator.
		[[nodiscard]] bool operator==(const part& other) const noexcept
		{
			return this->_owner.hwnd() == other._owner.hwnd()
				&& this->_index == other._index;
		}

		/// Inequality operator.
		[[nodiscard]] bool operator!=(const part& other) const noexcept { return !this->operator==(other); }

		/// Returns the wl::status_bar to which this part belongs.
		[[nodiscard]] const status_bar& owner() const noexcept { return this->_owner; }
		/// Returns the wl::list_view to which this part belongs.
		[[nodiscard]] status_bar&       owner() noexcept       { return this->_owner; }

		/// Returns the zero-based index of this part.
		[[nodiscard]] size_t index() const noexcept { return this->_index; }

		/// Sets the icon; pass nullptr to clear.
		/// The icon object is shared, and must remain valid.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sb-seticon
		const part& set_icon(HICON hIcon) const
		{
			if (SendMessageW(this->_owner.hwnd(), SB_SETICON,
				this->_index, reinterpret_cast<LPARAM>(hIcon) ) == 0)
			{
				throw std::runtime_error(
					str::unicode_to_ansi(
						str::format(L"SB_SETICON failed to part %d in " __FUNCTION__ "().",
							this->_index)));
			}
			return *this;
		}

		/// Sets the icon; pass nullptr to clear.
		/// The icon object is shared, and must remain valid.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sb-seticon
		const part& set_icon(const icon& ico) const
		{
			return this->set_icon(ico.hicon());
		}

		/// Sets the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sb-settext
		const part& set_text(std::wstring_view text) const
		{
			if (SendMessageW(this->_owner.hwnd(), SB_SETTEXT,
				MAKEWPARAM(MAKEWORD(this->_index, 0), 0),
				reinterpret_cast<LPARAM>(text.data()) ) == FALSE)
			{
				throw std::runtime_error(
					str::unicode_to_ansi(
						str::format(L"SB_SETTEXT failed for \"%s\" at %d, in " __FUNCTION__ "().",
							text, this->_index)));
			}
			return *this;
		}

		/// Retrieves the text.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sb-gettext
		[[nodiscard]] std::wstring text() const noexcept
		{
			WORD len = LOWORD(SendMessageW(
				this->_owner.hwnd(), SB_GETTEXTLENGTHW, this->_index, 0));

			std::wstring buf;
			if (len > 0) {
				buf.resize(static_cast<size_t>(len) + 1, L'\0');
				SendMessageW(this->_owner.hwnd(), SB_GETTEXT, this->_index,
					reinterpret_cast<LPARAM>(&buf[0]) );
				buf.resize(len);
			}
			return buf;
		}
	};

private:
	/// Provides access to the parts of a wl::status_bar.
	///
	/// You can't create this object, it's created internally by the status bar.
	/// 
	/// #include <status_bar.h>
	class part_collection final {
	private:
		friend status_bar;
		status_bar& _owner;

		explicit part_collection(status_bar& owner) noexcept
			: _owner{owner} { }

	public:
		/// Returns the part at the given index.
		/// @warning Does not perform bound checking.
		[[nodiscard]] part operator[](size_t itemIndex) const noexcept { return part{this->_owner, itemIndex}; }

		/// Adds a new part with fixed width.
		status_bar& add_fixed(UINT sizePixels)
		{
			this->_owner._partsData.push_back({sizePixels, 0});
			this->_owner._rightEdges.emplace_back(0);

			this->_owner.resize_to_fit_parent(msg::wm{
				SIZE_RESTORED,
				MAKELPARAM(this->_owner._parent_cx_during_creation(), 0)
			});
			return this->_owner;
		}

		/// Adds a new resizable part.
		///
		/// How resizeWeight works:
		/// - Suppose you have 3 parts, respectively with weights of 1, 1 and 2.
		/// - If available client area is 400px, respective part widths will be 100, 100 and 200px.
		status_bar& add_resizable(UINT resizeWeight)
		{
			if (resizeWeight == 0) {
				throw std::invalid_argument("Resize weight of status bar part can't be zero.");
			}

			// Zero weight means a fixed-width part, which internally should have sizePixels set.
			this->_owner._partsData.push_back({0, resizeWeight});
			this->_owner._rightEdges.emplace_back(0);

			this->_owner.resize_to_fit_parent(msg::wm{
				SIZE_RESTORED,
				MAKELPARAM(this->_owner._parent_cx_during_creation(), 0)
			});
			return this->_owner;
		}

		/// Returns the number of parts.
		[[nodiscard]] size_t count() const noexcept { return this->_owner._partsData.size(); }

		/// Sets the icon of multiple parts at once.
		/// The icon objects are shared, and must remain valid.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sb-seticon
		const status_bar& set_icons(std::initializer_list<HICON> hIcons) const
		{
			for (size_t i = 0; i < hIcons.size(); ++i) {
				this->operator[](i).set_icon(*(hIcons.begin() + i));
			}
			return this->_owner;
		}

		/// Sets the icon of multiple parts at once.
		/// The icon objects are shared, and must remain valid.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sb-seticon
		const status_bar& set_icons(
			std::initializer_list<std::reference_wrapper<const icon>> icons) const
		{
			for (size_t i = 0; i < icons.size(); ++i) {
				this->operator[](i).set_icon(*(icons.begin() + i));
			}
			return this->_owner;
		}

		/// Sets the text of multiple parts at once.
		/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sb-settext
		const status_bar& set_texts(std::initializer_list<std::wstring_view> texts) const
		{
			for (size_t i = 0; i < texts.size(); ++i) {
				this->operator[](i).set_text(*(texts.begin() + i));
			}
			return this->_owner;
		}
	};

	struct part_data final {
		UINT sizePixels = 0;
		UINT resizeWeight = 0;
	};

	_wli::base_native_control _base;
	msg::status_bar::wnd_events _events;
	adjust _adjustAction;
	_wli::base_parent_subclass<WM_SIZE, msg::wm_size> _parentResize;
	std::vector<part_data> _partsData;
	std::vector<int> _rightEdges; // buffer to speed up adjust() calls

public:
	/// Access to the status bar parts.
	part_collection parts{*this};

	/// Constructor, explicitly defining the control ID.
	status_bar(i_parent_window* parent, WORD ctrlId, adjust adjustAction = adjust::AUTO) noexcept
		: _base{*parent, ctrlId}, _events{_base}, _adjustAction{adjustAction} { }

	/// Constructor, with an auto-generated control ID.
	explicit status_bar(i_parent_window* parent, adjust adjustAction = adjust::AUTO) noexcept
		: _base{*parent}, _events{_base}, _adjustAction{adjustAction} { }

	/// Move constructor.
	status_bar(status_bar&&) = default;

	/// Move assignment operator.
	status_bar& operator=(status_bar&&) = default;

	/// Calls CreateWindowEx().
	/// @note Should be called during parent's WM_CREATE processing (or if
	/// dialog, WM_INITDIALOG).
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	status_bar& create()
	{
		LONG_PTR parentStyle = GetWindowLongPtrW(this->_base.parent().hwnd(), GWL_STYLE);
		bool isParentResizable = (parentStyle & WS_MAXIMIZEBOX) != 0
			|| (parentStyle & WS_SIZEBOX) != 0;

		this->_base.create_window(STATUSCLASSNAMEW, {}, {0, 0}, {0, 0},
			WS_CHILD | WS_VISIBLE | SBARS_TOOLTIPS | (isParentResizable ? SBARS_SIZEGRIP : 0),
			0);

		if (isParentResizable && this->_adjustAction == adjust::AUTO) {
			this->_parentResize.subclass(this->_base.parent().hwnd(), [this](msg::wm_size p) {
				this->resize_to_fit_parent(p);
			});
		}

		return *this;
	}

	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::status_bar::wnd_events& on() noexcept { return this->_events; }

	/// Exposes the subclassing handler methods. If at least one handle is added, the control will be subclassed.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/subclassing-overview
	[[nodiscard]] msg::wnd_events& on_subclass() { return this->_base.on_subclass(); }

	/// Returns the underlying HWND handle.
	[[nodiscard]] HWND hwnd() const noexcept override { return this->_base.hwnd(); }

	/// Returns the control ID.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdlgctrlid
	[[nodiscard]] WORD ctrl_id() const noexcept { return this->_base.ctrl_id(); } // no override, doesn't inherit from i_control

	/// Resizes the status bar to fill the available width on parent window.
	/// Intended to be called with parent's WM_SIZE processing.
	/// 
	/// If you are using wl::status_bar::adjust::AUTO, there's no need to call
	/// this method.
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-size
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sb-setparts
	status_bar& resize_to_fit_parent(msg::wm_size p) noexcept
	{
		if (!p.is_minimized() && this->hwnd() != nullptr) {
			int cx = p.client_area_size().cx; // available width
			SendMessageW(this->hwnd(), WM_SIZE, 0, 0); // tell status bar to fit parent

			// Find the space to be divided among variable-width parts,
			// and total weight of variable-width parts.
			UINT totalWeight = 0;
			int  cxVariable = cx;
			for (const part_data& onePartData : this->_partsData) {
				if (!onePartData.resizeWeight) { // fixed-width?
					cxVariable -= onePartData.sizePixels;
				} else {
					totalWeight += onePartData.resizeWeight;
				}
			}

			// Fill right edges array with the right edge of each part.
			int cxTotal = cx;
			for (size_t i = this->_partsData.size(); i-- > 0; ) {
				this->_rightEdges[i] = cxTotal;
				cxTotal -= (!this->_partsData[i].resizeWeight) ? // fixed-width?
					this->_partsData[i].sizePixels :
					static_cast<int>( (cxVariable / totalWeight) * this->_partsData[i].resizeWeight );
			}
			SendMessageW(this->hwnd(), SB_SETPARTS,
				this->_rightEdges.size(),
				reinterpret_cast<LPARAM>(&this->_rightEdges[0]) );
		}

		return *this;
	}

private:
	// Returns the width of parent client area.
	// Cached, since parts are intended to be added during window creation only,
	// and you can have only 1 status bar per window.
	[[nodiscard]] int _parent_cx_during_creation() noexcept
	{
		static int cx = 0;
		if (cx == 0 && this->hwnd() != nullptr) {
			RECT rc{};
			GetClientRect(GetParent(this->hwnd()), &rc);
			cx = rc.right; // cache
		}
		return cx;
	}
};

}//namespace wl