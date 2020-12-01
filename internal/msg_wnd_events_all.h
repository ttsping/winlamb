/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <Windows.h>
#include "depot.h"
#include "lambda_type.h"
#include "msg_wm.h"
#include "msg_wnd_events.h"

namespace wl::msg {

/// Extends msg::wnd_events, adding handler methods to WM_COMMAND messages
/// and WM_NOTIFY notifications.
//
/// A reference to this class is returned by methods like window_main::on().
///
/// @see @ref ex02
/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-notify
class wnd_events_all final : public wnd_events {
private:
	_wli::depot<WORD, WORD>& _depotCmd;
	_wli::depot<WORD, int>& _depotNfy;

public:
	wnd_events_all(_wli::depot<UINT>& depotWm, _wli::depot<UINT_PTR>& depotTimer,
		_wli::depot<WORD, WORD>& depotCmd, _wli::depot<WORD, int>& depotNfy, bool isDlg)
		: wnd_events{depotWm, depotTimer, isDlg}, _depotCmd{depotCmd}, _depotNfy{depotNfy} { }

	/// Adds a handler to WM_COMMAND, to a specific command ID and notification code.
	/// @note Always prefer the specific command handlers, which are safer.
	/// @tparam F `std::function<void(msg::wm_command)>` or `std::function<void()>`
	/// @param cmdId Command ID, same as `LOWORD(WPARAM)`.
	/// @param notifCode Notification code, same as `HIWORD(WPARAM)`.
	/// @param func `[](msg::wm_command p) {}` or `[]() {}`
	/// @see @ref ex02
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command(WORD cmdId, WORD notifCode, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command), void)
	{
		this->_depotCmd.add(cmdId, notifCode,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command(WORD cmdId, WORD notifCode, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdId, notifCode,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}
	/// Adds a handler to WM_COMMAND, to multiple command IDs and a notification code.
	/// @note Always prefer the specific command handlers, which are safer.
	/// @tparam F `std::function<void(msg::wm_command)>`
	/// @param cmdIds Multiple command IDs, same as `LOWORD(WPARAM)`.
	/// @param notifCode Notification code, same as `HIWORD(WPARAM)`.
	/// @param func: `[](msg::wm_command p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command(std::initializer_list<WORD> cmdIds, WORD notifCode, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command), void)
	{
		this->_depotCmd.add(cmdIds, notifCode,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command(std::initializer_list<WORD> cmdIds, int notifCode, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdIds, notifCode,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}
	/// Adds a handler to WM_COMMAND, to a command ID and multiple notification codes.
	/// @note Always prefer the specific command handlers, which are safer.
	/// @tparam F `std::function<void(msg::wm_command)>`
	/// @param cmdId Command ID, same as `LOWORD(WPARAM)`.
	/// @param notifCodes Multiple notification codes, same as `HIWORD(WPARAM)`.
	/// @param func: `[](msg::wm_command p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command(WORD cmdId, std::initializer_list<WORD> notifCodes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command), void)
	{
		this->_depotCmd.add(cmdId, notifCodes,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command(WORD cmdId, std::initializer_list<WORD> notifCodes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdId, notifCodes,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}
	/// Adds a handler to WM_COMMAND, to multiple command IDs and notification codes.
	/// @note Always prefer the specific command handlers, which are safer.
	/// @tparam F `std::function<void(msg::wm_command)>`
	/// @param cmdIds Multiple command IDs, same as `LOWORD(WPARAM)`.
	/// @param notifCodes Multiple notification codes, same as `HIWORD(WPARAM)`.
	/// @param func: `[](msg::wm_command p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command(std::initializer_list<WORD> cmdIds,
		std::initializer_list<WORD> notifCodes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_notify), void)
	{
		this->_depotCmd.add(cmdIds, notifCodes,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command(std::initializer_list<WORD> cmdIds,
		std::initializer_list<WORD> notifCodes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdIds, notifCodes,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}

	/// Adds a handler to WM_COMMAND for an accelerator notification, to a specific command ID.
	/// @tparam F `std::function<void(msg::wm_command_accel)>`
	/// @param cmdId Command ID, same as `LOWORD(WPARAM)`.
	/// @param func `[](msg::wm_command_accel p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command_accel(WORD cmdId, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command_accel), void)
	{
		this->_depotCmd.add(cmdId, 1,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command_accel(WORD cmdId, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdId, 1,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}
	/// Adds a handler to WM_COMMAND for an accelerator notification, to multiple command IDs.
	/// @tparam F `std::function<void(msg::wm_command_accel)>`
	/// @param cmdIds Multiple command IDs, same as `LOWORD(WPARAM)`.
	/// @param func `[](msg::wm_command_accel p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command_accel(std::initializer_list<WORD> cmdIds, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command_accel), void)
	{
		this->_depotCmd.add(cmdIds, 1,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command_accel(std::initializer_list<WORD> cmdIds, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdIds, 1,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}

	/// Adds a handler to WM_COMMAND for a menu notification, to a specific command ID.
	/// @tparam F `std::function<void(msg::wm_command_menu)>`
	/// @param cmdId Command ID, same as `LOWORD(WPARAM)`.
	/// @param func `[](msg::wm_command_menu p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command_menu(WORD cmdId, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command_menu), void)
	{
		this->_depotCmd.add(cmdId, 0,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command_menu(WORD cmdId, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdId, 0,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}
	/// Adds a handler to WM_COMMAND for a menu notification, to multiple command IDs.
	/// @tparam F `std::function<void(msg::wm_command_menu)>`
	/// @param cmdIds Multiple command IDs, same as `LOWORD(WPARAM)`.
	/// @param func `[](msg::wm_command_menu p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command_menu(std::initializer_list<WORD> cmdIds, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command_menu), void)
	{
		this->_depotCmd.add(cmdIds, 0,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command_menu(std::initializer_list<WORD> cmdIds, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdIds, 0,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}

	/// Adds a handler to WM_COMMAND for an accelerator or menu notification, to a specific command ID.
	/// @tparam F `std::function<void(msg::wm_command_accel_menu)>`
	/// @param cmdId Command ID, same as `LOWORD(WPARAM)`.
	/// @param func `[](msg::wm_command_accel_menu p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command_accel_menu(WORD cmdId, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command_accel_menu), void)
	{
		this->_depotCmd.add(cmdId, {0, 1},
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command_accel_menu(WORD cmdId, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdId, {0, 1},
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}
	/// Adds a handler to WM_COMMAND for an accelerator or menu notification, to multiple command IDs.
	/// @tparam F `std::function<void(msg::wm_command_accel_menu)>`
	/// @param cmdIds Multiple command IDs, same as `LOWORD(WPARAM)`.
	/// @param func `[](msg::wm_command_accel_menu p) {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
	template<typename F>
	auto wm_command_accel_menu(std::initializer_list<WORD> cmdIds, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_command_accel_menu), void)
	{
		this->_depotCmd.add(cmdIds, {0, 1},
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_command_accel_menu(std::initializer_list<WORD> cmdIds, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotCmd.add(cmdIds, {0, 1},
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}

	/// Adds a handler to WM_NOTIFY, to a specific control ID and notification code.
	/// @note Always prefer the specific notify handlers, which are safer.
	/// @tparam F `std::function<LRESULT(msg::wm_notify)>`
	/// @param ctrlId Control ID.
	/// @param notifCode Notification code.
	/// @param func: `[](msg::wm_notify p) -> LRESULT {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-notify
	template<typename F>
	auto wm_notify(WORD ctrlId, int notifCode, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(msg::wm_notify), void)
	{
		this->_depotNfy.add(ctrlId, notifCode,
			[func{std::forward<F>(func)}](msg::wm p) -> LRESULT { return func(p); });
	}
	template<typename F>
	auto wm_notify(WORD ctrlId, int notifCode, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(), void)
	{
		this->_depotNfy.add(ctrlId, notifCode,
			[func{std::forward<F>(func)}](msg::wm) -> LRESULT { return func(); });
	}
	/// Adds a handler to WM_NOTIFY, to multiple control IDs and a notification code.
	/// @note Always prefer the specific notify handlers, which are safer.
	/// @tparam F `std::function<LRESULT(msg::wm_notify)>`
	/// @param ctrlIds Multiple control IDs.
	/// @param notifCode Notification code.
	/// @param func: `[](msg::wm_notify p) -> LRESULT {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-notify
	template<typename F>
	auto wm_notify(std::initializer_list<WORD> ctrlIds, int notifCode, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(msg::wm_notify), void)
	{
		this->_depotNfy.add(ctrlIds, notifCode,
			[func{std::forward<F>(func)}](msg::wm p) -> LRESULT { return func(p); });
	}
	template<typename F>
	auto wm_notify(std::initializer_list<WORD> ctrlIds, int notifCode, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(), void)
	{
		this->_depotNfy.add(ctrlIds, notifCode,
			[func{std::forward<F>(func)}](msg::wm) -> LRESULT { return func(); });
	}
	/// Adds a handler to WM_NOTIFY, to a control ID and multiple notification codes.
	/// @note Always prefer the specific notify handlers, which are safer.
	/// @tparam F `std::function<LRESULT(msg::wm_notify)>`
	/// @param ctrlId Control ID.
	/// @param notifCodes Multiple notification codes.
	/// @param func: `[](msg::wm_notify p) -> LRESULT {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-notify
	template<typename F>
	auto wm_notify(WORD ctrlId, std::initializer_list<int> notifCodes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(msg::wm_notify), void)
	{
		this->_depotNfy.add(ctrlId, notifCodes,
			[func{std::forward<F>(func)}](msg::wm p) -> LRESULT { return func(p); });
	}
	template<typename F>
	auto wm_notify(WORD ctrlId, std::initializer_list<int> notifCodes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(), void)
	{
		this->_depotNfy.add(ctrlId, notifCodes,
			[func{std::forward<F>(func)}](msg::wm) -> LRESULT { return func(); });
	}
	/// Adds a handler to WM_NOTIFY, to multiple control IDs and notification codes.
	/// @note Always prefer the specific notify handlers, which are safer.
	/// @tparam F `std::function<LRESULT(msg::wm_notify)>`
	/// @param ctrlIds Multiple control IDs.
	/// @param notifCodes Multiple notification codes.
	/// @param func: `[](msg::wm_notify p) -> LRESULT {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-notify
	template<typename F>
	auto wm_notify(std::initializer_list<WORD> ctrlIds,
		std::initializer_list<int> notifCodes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(msg::wm_notify), void)
	{
		this->_depotNfy.add(ctrlIds, notifCodes,
			[func{std::forward<F>(func)}](msg::wm p) -> LRESULT { return func(p); });
	}
	template<typename F>
	auto wm_notify(std::initializer_list<WORD> ctrlIds,
		std::initializer_list<int> notifCodes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(), void)
	{
		this->_depotNfy.add(ctrlIds, notifCodes,
			[func{std::forward<F>(func)}](msg::wm) -> LRESULT { return func(); });
	}
};

}//namespace wl::msg