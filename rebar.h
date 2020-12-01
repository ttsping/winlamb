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
#include "internal/interfaces.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Native rebar control.
///
/// #include <rebar.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rebar-controls
class rebar final : public i_control {
public:
	/// Determines if the rebar will be resized automatically when the parent is
	/// resized.
	enum class adjust {
		/// The rebar will be automatically resized when the parend is resized.
		/// This is done by subclassing the parent window.
		AUTO,
		/// The rebar will be resized only when you manually call
		/// resize_to_fit_parent(), preferably during parent's WM_SIZE processing.
		MANUAL
	};

private:
	_wli::base_native_control _base;
	msg::rebar::wnd_events _events;
	adjust _adjustAction;
	_wli::base_parent_subclass<WM_SIZE, msg::wm_size> _parentResize;

public:
	/// Constructor, explicitly defining the control ID.
	rebar(i_parent_window* parent, WORD ctrlId, adjust adjustAction = adjust::AUTO) noexcept
		: _base{*parent, ctrlId}, _events{_base}, _adjustAction{adjustAction} { }

	/// Constructor, with an auto-generated control ID.
	explicit rebar(i_parent_window* parent, adjust adjustAction = adjust::AUTO) noexcept
		: _base{*parent}, _events{_base}, _adjustAction{adjustAction} { }

	/// Move constructor.
	rebar(rebar&&) = default;

	/// Move assignment operator.
	rebar& operator=(rebar&&) = default;

	/// Calls CreateWindowEx().
	/// @note Should be called during parent's WM_CREATE processing (or if
	/// dialog, WM_INITDIALOG).
	/// @param rbStyles Rebar control styles. RBS_ and CCS_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rebar-control-styles
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/common-control-styles
	rebar& create(
		DWORD rbStyles = RBS_VARHEIGHT | CCS_NODIVIDER)
	{
		this->_base.create_window(REBARCLASSNAMEW, {}, {0, 0}, {0, 0},
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | rbStyles,
			0);

		LONG_PTR parentStyle = GetWindowLongPtrW(this->_base.parent().hwnd(), GWL_STYLE);
		bool isParentResizable = (parentStyle & WS_MAXIMIZEBOX) != 0
			|| (parentStyle & WS_SIZEBOX) != 0;

		if (isParentResizable && this->_adjustAction == adjust::AUTO) {
			this->_parentResize.subclass(this->_base.parent().hwnd(), [this](msg::wm_size p) {
				this->resize_to_fit_parent();
			});
		}

		return *this;
	}

	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::rebar::wnd_events& on() noexcept { return this->_events; }

	/// Exposes the subclassing handler methods. If at least one handle is added, the control will be subclassed.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/subclassing-overview
	[[nodiscard]] msg::wnd_events& on_subclass() { return this->_base.on_subclass(); }

	/// Returns the underlying HWND handle.
	[[nodiscard]] HWND hwnd() const noexcept override { return this->_base.hwnd(); }

	/// Returns the control ID.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdlgctrlid
	[[nodiscard]] WORD ctrl_id() const noexcept { return this->_base.ctrl_id(); } // no override, doesn't inherit from i_control

	/// Adds a new band, moving the child control into it.
	/// @param text Optional text of the band.
	/// @param child Child control to be taken.
	/// @param style Rebar band styles. RBBS_ constants.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rb-insertband
	const rebar& add(std::wstring_view text, const i_control& child,
		DWORD styles = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS) const
	{
		RECT rcChild{};
		wchar_t className[32]{};
		GetClassNameW(child.hwnd(), className, ARRAYSIZE(className));
		if (!lstrcmpW(className, TOOLBARCLASSNAMEW)) {
			// For a tool bar, we get the dimensions based on its buttons.
			DWORD nBtn = static_cast<DWORD>(SendMessageW(child.hwnd(), TB_BUTTONCOUNT, 0, 0));
			DWORD szBtn = static_cast<DWORD>(SendMessageW(child.hwnd(), TB_GETBUTTONSIZE, 0, 0));
			rcChild.right = LOWORD(szBtn) * nBtn;
			rcChild.bottom = HIWORD(szBtn);
		} else {
			// For any other control, we simple take the window dimensions.
			GetWindowRect(child.hwnd(), &rcChild);
		}

		DWORD mask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE
			| (text.length() ? RBBIM_TEXT : 0);

		REBARBANDINFOW rbi{};
		rbi.cbSize = sizeof(REBARBANDINFOW);
		rbi.fMask = mask;
		rbi.fStyle = styles;
		rbi.lpText = const_cast<wchar_t*>(text.data());
		rbi.hwndChild = child.hwnd();
		rbi.cyMinChild = rcChild.bottom - rcChild.top;
		rbi.cx = rcChild.right - rcChild.left;

		if (!SendMessageW(this->hwnd(), RB_INSERTBANDW, -1, reinterpret_cast<LPARAM>(&rbi))) {
			throw std::runtime_error("RB_INSERTBAND failed.");
		}
		return *this;
	}

	/// Resizes the rebar to fill the available width on parent window.
	/// Intended to be called with parent's WM_SIZE processing.
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-size
	const rebar& resize_to_fit_parent() const noexcept
	{
		// https://groups.google.com/g/comp.os.ms-windows.programmer.win32/c/UFq6z2XWi2Q
		SendMessageW(this->hwnd(), WM_SIZE, 0, 0);
		return *this;
	}

	/// Retrieves the number of bands in the rebar.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rb-getbandcount
	[[nodiscard]] size_t band_count() const noexcept { return SendMessageW(this->hwnd(), RB_GETBANDCOUNT, 0, 0); }

	/// Retrieves the number of rows in the rebar.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rb-getbandcount
	[[nodiscard]] size_t row_count() const noexcept { return SendMessageW(this->hwnd(), RB_GETROWCOUNT, 0, 0); }

	/// Retrieves the height of the rebar, in pixels.
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rb-getbandcount
	[[nodiscard]] size_t height() const noexcept { return SendMessageW(this->hwnd(), RB_GETBARHEIGHT, 0, 0); }
};

}//namespace wl