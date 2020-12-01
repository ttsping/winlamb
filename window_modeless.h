/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <string_view>
#include <Windows.h>
#include "internal/base_main_loop.h"
#include "internal/base_window.h"
#include "internal/control_visuals.h"
#include "internal/interfaces.h"
#include "internal/str_aux.h"

namespace wl {

/// Modeless popup window.
/// Allows message and notification handling.
///
/// #include <window_modeless.h>
///
/// @note The following messages are default handled. If you add a handler to
/// any of them, you'll overwrite the default behavior:
/// - msg::wnd_events::wm_close()
/// - msg::wnd_events::wm_nc_destroy()
class window_modeless : public i_parent_window {
public:
	/// Setup options for window_modeless.
	struct setup_opts final {
		/// Window class name to be registered. Defaults to an auto-generated string.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexw
		std::wstring class_name;
		/// Window class styles.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexw
		DWORD class_style = CS_DBLCLKS;
		/// Window HCURSOR. If none is specified, will load IDC_ARROW.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexw
		HCURSOR class_cursor = nullptr;
		/// Window background HBRUSH. Defaults to brown/gray.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexw
		HBRUSH class_bg_brush = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);

		/// Window title. Defaults to an empty string.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
		std::wstring title;
		/// Size of window client area, which does not include title bar or borders.
		/// Will be adjusted to match current system DPI.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-adjustwindowrectex
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
		SIZE size = {300, 200};
		/// Window extended styles.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
		DWORD ex_style = WS_EX_TOOLWINDOW;
		/// Window styles.
		/// Suggestion: WS_SYSMENU (X button).
		DWORD style = WS_CAPTION | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE;
	};

private:
	setup_opts _setup;
	_wli::base_window _base;

public:
	/// Default constructor.
	window_modeless()
	{
		this->_default_msg_handlers();
	}

	/// Move constructor.
	window_modeless(window_modeless&&) = default;

	/// Move assignment operator.
	window_modeless& operator=(window_modeless&&) = default;

	/// Creates the window and returns immediately. Position will be adjusted to match current system DPI.
	///
	/// Should be called during parent's WM_CREATE processing (or if dialog, WM_INITDIALOG).
	///
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	virtual void create(const i_window* parent, POINT pos = {0, 0})
	{
		if (parent == nullptr) {
			throw std::invalid_argument("No parent passed in " __FUNCTION__ "().");
		}

		HINSTANCE hInst = reinterpret_cast<HINSTANCE>(
			GetWindowLongPtrW(parent->hwnd(), GWLP_HINSTANCE));
		WNDCLASSEXW wcx = this->_wcx_from_opts(hInst);
		this->_base.register_class(wcx);

		this->_setup.size = _wli::control_visuals::multiply_dpi(this->_setup.size);

		RECT rc = {
			0, 0, // left, top
			this->_setup.size.cx, // right
			this->_setup.size.cy  // bottom
		};
		if (AdjustWindowRectEx(&rc, this->_setup.style, FALSE, this->_setup.ex_style) == FALSE) {
			throw std::runtime_error("AdjustWindowRectEx() failed in " __FUNCTION__ "().");
		}
		this->_setup.size = {rc.right - rc.left, rc.bottom - rc.top};

		ClientToScreen(parent->hwnd(), &pos);

		HWND h = this->_base.create_window(wcx.hInstance, parent,
			wcx.lpszClassName, this->_setup.title, nullptr, pos, this->_setup.size,
			this->_setup.ex_style, this->_setup.style);

		SendMessageW(parent->hwnd(), _wli::WM_MODELESS_CREATED, // tell parent we're here
			0xc0de'f00d, reinterpret_cast<LPARAM>(h) );
	}

protected:
	/// Exposes variables that will be used in RegisterClassEx() and
	/// CreateWindowEx() calls, during window creation.
	/// @warning If you call this method after the window is created, an exception will be thrown.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	[[nodiscard]] setup_opts& setup()
	{
		if (this->hwnd() != nullptr) {
			throw std::logic_error("Cannot call setup() after window_modeless is created.");
		}
		return this->_setup;
	}

	/// Exposes the handler methods.
	/// @warning If you call this method after the window is created, an exception will be thrown.
	[[nodiscard]] msg::wnd_events_all& on() { return this->_base.on(); }

	/// Executes a function asynchronously, in a new detached background thread.
	/// @tparam F `std::function<void(ui_work)>`
	/// @param func `[](ui_work ui) {}`
	/// @see @ref ex09
	template<typename F>
	void background_work(F&& func) { this->_base.background_work(std::move(func)); }

	/// Sets the window title.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowtextw
	const window_modeless& set_title(std::wstring_view t) const noexcept { SetWindowTextW(this->hwnd(), t.data()); return *this; }

	/// Retrieves the window title.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtextw
	[[nodiscard]] std::wstring title() const { return _wli::str_aux::get_window_text(this->hwnd()); }

public:
	/// Returns the underlying HWND handle.
	[[nodiscard]] HWND hwnd() const noexcept override { return this->_base.hwnd(); }

private:
	void _default_msg_handlers()
	{
		this->on().wm_close([this]() noexcept
		{
			DestroyWindow(this->hwnd());
		});

		this->on().wm_nc_destroy([this]() noexcept
		{
			SendMessageW(GetWindow(this->hwnd(), GW_OWNER), // tell parent we're gone
				_wli::WM_MODELESS_DESTROYED,
				0xc0de'f00d, reinterpret_cast<LPARAM>(this->hwnd()) );
		});
	}

	[[nodiscard]] WNDCLASSEXW _wcx_from_opts(HINSTANCE hInst)
	{
		WNDCLASSEXW wcx{};
		wcx.cbSize = sizeof(WNDCLASSEXW);
		wcx.hInstance = hInst;
		wcx.style = this->_setup.class_style;
		wcx.hbrBackground = this->_setup.class_bg_brush;

		_wli::base_window::wcx_set_cursor(this->_setup.class_cursor, wcx);

		if (this->_setup.class_name.empty()) { // if user didn't choose a class name
			this->_setup.class_name = _wli::base_window::wcx_generate_hash(wcx); // generate hash after all fields are set
			wcx.lpszClassName = this->_setup.class_name.c_str();
		}
		return wcx;
	}
};

}//namespace wl