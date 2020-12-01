/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <Windows.h>
#include "msg_wm.h"
#include "msg_wnd_events_all.h"

namespace _wli { class base_dialog; } // forward declaration

namespace wl {

/// Abstract class, base to all windows.
class i_window {
public:
	virtual ~i_window() { }

	/// Returns the underlying HWND handle.
	[[nodiscard]] virtual HWND hwnd() const noexcept = 0;
};

/// Abstract class, base to all windows which can have controls.
class i_parent_window : public virtual i_window {
public:
	virtual ~i_parent_window() { }

	/// Exposes methods to add message handlers.
	/// @warning If you call this method after the window is created, an exception will be thrown.
	[[nodiscard]] virtual msg::wnd_events_all& on() = 0;
};

/// Abstract class, base to all controls.
class i_control : public virtual i_window {
public:
	virtual ~i_control() { }

	/// Returns the control ID.
	[[nodiscard]] virtual WORD ctrl_id() const noexcept = 0;
};

/// Abstract class, base to all controls which can be added to wl::resizer.
class i_resizable_control : public i_control {
public:
	virtual ~i_resizable_control() { }
};

/// Abstract class, base to all controls of a dialog box, which can be designed
/// using a resource editor.
class i_resource_control {
private:
	friend _wli::base_dialog;
	virtual void create_in_dialog() = 0;

public:
	virtual ~i_resource_control() { }
};

}//namespace wl