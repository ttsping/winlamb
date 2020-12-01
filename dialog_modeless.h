/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <string_view>
#include <Windows.h>
#include "internal/base_dialog.h"
#include "internal/base_main_loop.h"
#include "internal/interfaces.h"
#include "internal/str_aux.h"

namespace wl {

/// Modeless popup dialog.
/// Allows message and notification handling.
///
/// #include <dialog_modeless.h>
///
/// @note The following messages are default handled. If you add a handler to
/// any of them, you'll overwrite the default behavior:
/// - msg::wnd_events::wm_close()
/// - msg::wnd_events::wm_nc_destroy()
class dialog_modeless : public i_parent_window {
public:
	/// Setup options for dialog_modeless.
	struct setup_opts final {
		/// Resource dialog ID, must be set.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-dialogboxparamw
		int dialog_id = 0;
	};

private:
	setup_opts _setup;
	_wli::base_dialog _base;

public:
	/// Default constructor.
	dialog_modeless()
	{
		this->_default_msg_handlers();
	}

	/// Move constructor.
	dialog_modeless(dialog_modeless&&) = default;

	/// Move assignment operator.
	dialog_modeless& operator=(dialog_modeless&&) = default;

	/// Creates the modeless dialog and returns immediately.
	///
	/// Should be called during parent's WM_CREATE processing (or if dialog, WM_INITDIALOG).
	///
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createdialogparamw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	virtual void create(const i_window* parent)
	{
		if (parent == nullptr) {
			throw std::invalid_argument("No parent passed in " __FUNCTION__ "().");
		}

		HINSTANCE hInst = reinterpret_cast<HINSTANCE>(
			GetWindowLongPtrW(parent->hwnd(), GWLP_HINSTANCE));
		HWND h = this->_base.create_dialog_param(hInst, nullptr, this->_setup.dialog_id);

		SendMessageW(parent->hwnd(), _wli::WM_MODELESS_CREATED, // tell parent we're here
			0xc0de'f00d, reinterpret_cast<LPARAM>(h) );
	}

protected:
	/// Exposes variables that will be used during dialog creation.
	/// @warning If you call this method after the dialog is created, an exception will be thrown.
	setup_opts& setup()
	{
		if (this->hwnd() != nullptr) {
			throw std::logic_error("Cannot call setup() after dialog_modeless is created.");
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
	const dialog_modeless& set_title(std::wstring_view t) const noexcept { SetWindowTextW(this->hwnd(), t.data()); return *this; }

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
};

}//namespace wl