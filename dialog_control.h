/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <Windows.h>
#include "internal/base_native_control.h"
#include "internal/base_dialog.h"
#include "internal/control_visuals.h"
#include "internal/interfaces.h"

namespace wl {

/// Custom dialog-based user control.
/// Allows message and notification handling.
///
/// #include <dialog_control.h>
///
/// @note The following messages are default handled. If you add a handler to
/// any of them, you'll overwrite the default behavior:
/// - msg::wnd_events::wm_nc_paint()
///
/// @warning You must set these styles in the resource editor:
/// - Border: none
/// - Control: true
/// - Style: child
/// - Visible: true (otherwise will start invisible)
/// - Client Edge: true (only if you want a border)
class dialog_control : public virtual i_resizable_control, public virtual i_parent_window {
public:
	/// Setup options for dialog_control.
	struct setup_opts final {
		/// Resource dialog ID, must be set.
		/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createdialogparamw
		int dialog_id = 0;
	};

private:
	setup_opts _setup;
	_wli::base_dialog _base;

public:
	/// Default constructor.
	dialog_control()
	{
		this->_default_msg_handlers();
	}

	/// Move constructor.
	dialog_control(dialog_control&&) = default;

	/// Move assignment operator.
	dialog_control& operator=(dialog_control&&) = default;

	/// Creates the dialog control. Should be called during parent's WM_CREATE
	/// processing (or if dialog, WM_INITDIALOG).
	///
	/// @param parent The parent window.
	/// @param ctrlId The control ID to be given to this control.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system
	/// DPI. A size of {0, 0} will use the size defined in the dialog resource.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createdialogparamw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	virtual void create(const i_window* parent, WORD ctrlId, POINT pos, SIZE size = {0, 0})
	{
		if (parent == nullptr) {
			throw std::invalid_argument("No parent passed to  " __FUNCTION__ "().");
		}

		HINSTANCE hInst = reinterpret_cast<HINSTANCE>(
			GetWindowLongPtrW(parent->hwnd(), GWLP_HINSTANCE));
		this->_base.create_dialog_param(hInst, parent, this->_setup.dialog_id);

		SetWindowLongPtrW(this->hwnd(), GWLP_ID, ctrlId); // so the control has an ID

		bool useResourceSize = (size.cx == 0) && (size.cy == 0);
		SetWindowPos(this->hwnd(), nullptr, pos.x, pos.y, size.cx, size.cy,
			SWP_NOZORDER | (useResourceSize ? SWP_NOSIZE : 0));
	}

	/// Creates the dialog control, with an auto-generated control ID. Should be
	/// called during parent's WM_CREATE processing (or if dialog, WM_INITDIALOG).
	///
	/// @param parent The parent window.
	/// @param pos Position within parent client area. Will be adjusted to match current system DPI.
	/// @param size Size of the control. Will be adjusted to match current system
	/// DPI. A size of {0, 0} will use the size defined in the dialog resource.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createdialogparamw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	virtual void create(const i_window* parent, POINT pos, SIZE size = {0, 0})
	{
		this->create(parent, _wli::next_auto_ctrl_id(), pos, size);
	}

protected:
	/// Exposes variables that will be used during dialog creation.
	/// @warning If you call this method after the dialog is created, an exception will be thrown.
	[[nodiscard]] setup_opts& setup()
	{
		if (this->hwnd() != nullptr) {
			throw std::logic_error("Cannot call setup() after dialog_control is created.");
		}
		return this->_setup;
	}

	/// Exposes the handler methods.
	/// @warning If you call this method after the control is created, an exception will be thrown.
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

public:
	/// Returns the underlying HWND handle.
	[[nodiscard]] HWND hwnd() const noexcept override { return this->_base.hwnd(); }

	/// Retrieves the control ID.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdlgctrlid
	[[nodiscard]] WORD ctrl_id() const noexcept override { return GetDlgCtrlID(this->hwnd()); }

private:
	void _default_msg_handlers()
	{
		this->on().wm_nc_paint([this](msg::wm_nc_paint p) noexcept
		{
			_wli::control_visuals::paint_control_borders(this->hwnd(), p.wparam, p.lparam);
		});
	}
};

}//namespace wl