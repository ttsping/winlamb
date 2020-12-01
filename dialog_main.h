/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <Windows.h>
#include <CommCtrl.h>
#include <VersionHelpers.h>
#include "internal/base_dialog.h"
#include "internal/base_main_loop.h"
#include "internal/gdi_obj.h"
#include "internal/interfaces.h"
#include "internal/str_aux.h"

namespace wl {

/// Dialog to be used as the application main window.
/// Allows message and notification handling.
///
/// #include <dialog_main.h>
///
/// @note The following messages are default handled. If you add a handler to
/// any of them, you'll overwrite the default behavior:
/// - msg::wnd_events::wm_close()
/// - msg::wnd_events::wm_nc_destroy()
///
/// @see @ref ex05
class dialog_main : public i_parent_window {
public:
	/// Setup options for dialog_main.
	/// @see @ref ex05
	struct setup_opts final {
		/// Resource dialog ID, must be set.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createdialogparamw
		int dialog_id = 0;
		/// Resource icon ID, optional.
		/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-seticon
		int icon_id = 0;
		/// Resource accelerator table ID, optional.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadacceleratorsw
		int accel_tbl_id = 0;
	};

private:
	setup_opts _setup;
	_wli::base_dialog _base;
	_wli::base_main_loop _mainLoop;

public:
	/// Default constructor.
	dialog_main()
	{
		this->_default_msg_handlers();
	}

	/// Move constructor.
	dialog_main(dialog_main&&) = default;

	/// Move assignment operator.
	dialog_main& operator=(dialog_main&&) = default;

	/// Creates the dialog window and runs the main application loop.
	///
	/// If you need any custom code to run before the window is created, you can
	/// override this method.
	///
	/// @note Prefer using the RUN macro, which does the following:
	/// - creates WinMain() entry point;
	/// - instantiates your main dialog;
	/// - calls run_as_main() automatically.
	///
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createdialogparamw
	virtual int run_as_main(HINSTANCE hInst, int cmdShow = SW_SHOW)
	{
		if (IsWindowsVistaOrGreater()) {
			SetProcessDPIAware();
		}
		InitCommonControls();
		_wli::globalUiFont.create_ui();

		this->_base.create_dialog_param(hInst, nullptr, this->_setup.dialog_id);

		HACCEL hAccel = nullptr;
		if (this->_setup.accel_tbl_id != 0) {
			// An accelerator table loaded from resource is automatically freed by the system.
			hAccel = LoadAcceleratorsW(hInst, MAKEINTRESOURCEW(this->_setup.accel_tbl_id));
			if (hAccel == nullptr) {
				throw std::system_error(GetLastError(), std::system_category(),
					"LoadAccelerators() failed  in " __FUNCTION__ "().");
			}
		}

		this->_set_icon_if_any(hInst);
		ShowWindow(this->hwnd(), cmdShow);
		return this->_mainLoop.run_loop(this->hwnd(), hAccel);
	}

protected:
	/// Exposes variables that will be used during dialog creation.
	/// @warning If you call this method after the dialog is created, an exception will be thrown.
	[[nodiscard]] setup_opts& setup()
	{
		if (this->hwnd() != nullptr) {
			throw std::logic_error("Cannot call setup() after dialog_main is created.");
		}
		return this->_setup;
	}

	/// Exposes the handler methods.
	/// @warning If you call this method after the window is created, an exception will be thrown.
	[[nodiscard]] msg::wnd_events_all& on() { return this->_base.on(); }

	/// Creates one or more child controls, which must exist in the dialog resource.
	/// This method should be called during WM_INITDIALOG.
	/// @param children Each child control to be created.
	void create_children(
		std::initializer_list<std::reference_wrapper<i_resource_control>> children)
	{
		this->_base.create_children(children);
	}

	/// Executes a function asynchronously, in a new detached background thread.
	/// @tparam F `std::function<void(ui_work)>`
	/// @param func `[](ui_work ui) {}`
	/// @see @ref ex09
	template<typename F>
	void background_work(F&& func) { this->_base.background_work(std::move(func)); }

	/// Sets the window title.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowtextw
	const dialog_main& set_title(std::wstring_view t) const noexcept { SetWindowTextW(this->hwnd(), t.data()); return *this; }

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

		this->on().wm_nc_destroy([]() noexcept
		{
			PostQuitMessage(0);
		});
	}

	void _set_icon_if_any(HINSTANCE hInst) const noexcept
	{
		// If an icon ID was specified, load it from the resources.
		// Resource icons are automatically released by the system.
		if (this->_setup.icon_id != 0) {
			SendMessageW(this->hwnd(), WM_SETICON, ICON_SMALL,
				reinterpret_cast<LPARAM>(reinterpret_cast<HICON>(LoadImageW(hInst,
					MAKEINTRESOURCEW(this->_setup.icon_id),
					IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR))));

			SendMessageW(this->hwnd(), WM_SETICON, ICON_BIG,
				reinterpret_cast<LPARAM>(reinterpret_cast<HICON>(LoadImageW(hInst,
					MAKEINTRESOURCEW(this->_setup.icon_id),
					IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR))));
		}
	}
};

}//namespace wl