/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <Windows.h>
#include "depot.h"
#include "lambda_type.h"
#include "msg_wm.h"

namespace wl::msg {

/// Exposes handler methods to windows messages.
///
/// A reference to this class is returned by methods like
/// window_main::on() and button::on_subclass().
///
/// @see @ref ex02
/// @see https://docs.microsoft.com/en-us/windows/win32/learnwin32/window-messages
class wnd_events {
private:
	_wli::depot<UINT>& _depotWm;
	_wli::depot<UINT_PTR>& _depotTimer;

protected:
	bool _isDlg;

public:
	wnd_events(_wli::depot<UINT>& depotWm, _wli::depot<UINT_PTR>& depotTimer, bool isDlg)
		: _depotWm{depotWm}, _depotTimer{depotTimer}, _isDlg{isDlg} { }

	/// Adds a handler to an arbitrary window message.
	/// @note Always prefer the specific message handlers, which are safer.
	/// @warning WM_COMMAND, WM_NOTIFY and WM_TIMER will not work, use the specific handlers.
	/// @tparam F `std::function<LRESULT(msg::wm)>` or `std::function<LRESULT()>`
	/// @param message The message to be handled.
	/// @param func: `[](msg::wm p) -> LRESULT {}` or `[]() -> LRESULT {}`
	template<typename F>
	auto wm(UINT message, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(msg::wm), void)
	{
		this->_depotWm.add(message, std::forward<F>(func));
	}
	template<typename F>
	auto wm(UINT message, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(), void)
	{
		this->_depotWm.add(message,
			[func{std::forward<F>(func)}](msg::wm) -> LRESULT { return func(); });
	}
	/// Adds a single handler to many arbitrary window messages.
	/// @note Always prefer the specific message handlers, which are safer.
	/// @warning WM_COMMAND, WM_NOTIFY and WM_TIMER will not work, use the specific handlers.
	/// @tparam F `std::function<LRESULT(msg::wm)>` or `std::function<LRESULT()>`
	/// @param messages Multiple messages to be handled.
	/// @param func: `[](msg::wm p) -> LRESULT {}` or `[]() -> LRESULT {}`
	template<typename F>
	auto wm(std::initializer_list<UINT> messages, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(msg::wm), void)
	{
		this->_depotWm.add(messages, std::forward<F>(func));
	}
	template<typename F>
	auto wm(std::initializer_list<UINT> messages, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(), void)
	{
		this->_depotWm.add(messages,
			[func{std::forward<F>(func)}](msg::wm) -> LRESULT { return func(); });
	}

	/// Adds a handler to a specific timer ID of WM_TIMER.
	/// @tparam F `std::function<void(msg::wm_timer)>` or `std::function<void()>`
	/// @param timerId Timer ID, passed to SetTimer() call.
	/// @param func: `[](msg::wm_timer p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-timer
	template<typename F>
	auto wm_timer(UINT_PTR timerId, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_timer), void)
	{
		this->_depotTimer.add(timerId,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_timer(UINT_PTR timerId, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotTimer.add(timerId,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}
	/// Adds a handler to many timer IDs of WM_TIMER.
	/// @tparam F `std::function<void(msg::wm_timer)>` or `std::function<void()>`
	/// @param timerIds Multiple timer IDs, passed to SetTimer() call.
	/// @param func: `[](msg::wm_timer p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-timer
	template<typename F>
	auto wm_timer(std::initializer_list<UINT_PTR> timerIds, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::wm_timer), void)
	{
		this->_depotTimer.add(timerIds,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT {
				func(p);
				return isDlg ? TRUE : 0;
			});
	}
	template<typename F>
	auto wm_timer(std::initializer_list<UINT_PTR> timerIds, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		this->_depotTimer.add(timerIds,
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT {
				func();
				return isDlg ? TRUE : 0;
			});
	}

// The WM handler returns a predetermined value.
#define WINLAMB_MSG_RET_VALUE(methodName, dwMsg, retVal) \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, void(msg::methodName), void) \
	{ \
		this->_depotWm.add(dwMsg, \
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm p) -> LRESULT { \
				func(p); \
				return retVal; \
			}); \
	} \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, void(), void) \
	{ \
		this->_depotWm.add(dwMsg, \
			[func{std::forward<F>(func)}, isDlg{this->_isDlg}](msg::wm) -> LRESULT { \
				func(); \
				return retVal; \
			}); \
	}

// The WM handler returns zero, or TRUE if dialog.
#define WINLAMB_MSG_RET_DEFAULT(methodName, dwMsg) \
	WINLAMB_MSG_RET_VALUE(methodName, dwMsg, isDlg ? TRUE : 0)

// The WM handler returns an user user value, of the predetermined type.
#define WINLAMB_MSG_RET_TYPE(methodName, dwMsg, retType) \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, retType(msg::methodName), void) \
	{ \
		this->_depotWm.add(dwMsg, \
			[func{std::forward<F>(func)}](msg::wm p) -> LRESULT { return func(p); }); \
	} \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, retType(), void) \
	{ \
		this->_depotWm.add(dwMsg, \
			[func{std::forward<F>(func)}](msg::wm) -> LRESULT { return func(); }); \
	}

	/// Adds a handler to WM_ACTIVATE message.
	///
	/// @note Default handled in:
	/// - window_main
	///
	/// @tparam F `std::function<void(msg::wm_activate)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_activate p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-activate
	WINLAMB_MSG_RET_DEFAULT(wm_activate, WM_ACTIVATE)

	/// Adds a handler to WM_ACTIVATEAPP message.
	/// @tparam F `std::function<void(msg::wm_activate_app)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_activate_app p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-activateapp
	WINLAMB_MSG_RET_DEFAULT(wm_activate_app, WM_ACTIVATEAPP)

	/// Adds a handler to WM_APPCOMMAND message.
	/// @tparam F `std::function<void(msg::wm_app_command)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_app_command p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-appcommand
	WINLAMB_MSG_RET_VALUE(wm_app_command, WM_APPCOMMAND, TRUE)

	/// Adds a handler to WM_ASKCBFORMATNAME message.
	/// @tparam F `std::function<void(msg::wm_ask_cb_format_name)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_ask_cb_format_name p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-askcbformatname
	WINLAMB_MSG_RET_DEFAULT(wm_ask_cb_format_name, WM_ASKCBFORMATNAME)

	/// Adds a handler to WM_CANCELMODE message.
	/// @tparam F `std::function<void(msg::wm_cancel_mode)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_cancel_mode p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-cancelmode
	WINLAMB_MSG_RET_DEFAULT(wm_cancel_mode, WM_CANCELMODE)

	/// Adds a handler to WM_CAPTURECHANGED message.
	/// @tparam F `std::function<void(msg::wm_capture_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_capture_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-capturechanged
	WINLAMB_MSG_RET_DEFAULT(wm_capture_changed, WM_CAPTURECHANGED)

	/// Adds a handler to WM_CHANGECBCHAIN message.
	/// @tparam F `std::function<void(msg::wm_change_cb_chain)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_change_cb_chain p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-changecbchain
	WINLAMB_MSG_RET_DEFAULT(wm_change_cb_chain, WM_CHANGECBCHAIN)

	/// Adds a handler to WM_CHAR message.
	/// @tparam F `std::function<void(msg::wm_char)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_char p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-char
	WINLAMB_MSG_RET_DEFAULT(wm_char, WM_CHAR)

	/// Adds a handler to WM_CHARTOITEM message.
	/// @tparam F `std::function<int(msg::wm_char_to_item)>` or `std::function<int()>`
	/// @param func: `[](msg::wm_char_to_item p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-chartoitem
	WINLAMB_MSG_RET_TYPE(wm_char_to_item, WM_CHARTOITEM, int)

	/// Adds a handler to WM_CHILDACTIVATE message.
	/// @tparam F `std::function<void(msg::wm_child_activate)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_child_activate p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-childactivate
	WINLAMB_MSG_RET_DEFAULT(wm_child_activate, WM_CHILDACTIVATE)

	/// Adds a handler to WM_CLIPBOARDUPDATE message.
	/// @tparam F `std::function<void(msg::wm_clipboard_update)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_clipboard_update p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-clipboardupdate
	WINLAMB_MSG_RET_DEFAULT(wm_clipboard_update, WM_CLIPBOARDUPDATE)

	/// Adds a handler to WM_CLOSE message.
	///
	/// @note Default handled in:
	/// - window_modal
	/// - window_modeless
	/// - dialog_main
	/// - dialog_modal
	/// - dialog_modeless
	///
	/// @tparam F `std::function<void(msg::wm_close)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_close p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-close
	WINLAMB_MSG_RET_DEFAULT(wm_close, WM_CLOSE)

	/// Adds a handler to WM_COMPACTING message.
	/// @tparam F `std::function<void(msg::wm_compacting)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_compacting p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-compacting
	WINLAMB_MSG_RET_DEFAULT(wm_compacting, WM_COMPACTING)

	/// Adds a handler to WM_COMPAREITEM message.
	/// @tparam F `std::function<int(msg::wm_compare_item)>` or `std::function<int()>`
	/// @param func: `[](msg::wm_compare_item p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-compareitem
	WINLAMB_MSG_RET_TYPE(wm_compare_item, WM_COMPAREITEM, int)

	/// Adds a handler to WM_CONTEXTMENU message.
	/// @tparam F `std::function<void(msg::wm_context_menu)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_context_menu p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-contextmenu
	WINLAMB_MSG_RET_DEFAULT(wm_context_menu, WM_CONTEXTMENU)

	/// Adds a handler to WM_COPYDATA message.
	/// @tparam F `std::function<bool(msg::wm_copy_data)>` or `std::function<bool()>`
	/// @param func: `[](msg::wm_copy_data p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-copydata
	WINLAMB_MSG_RET_TYPE(wm_copy_data, WM_COPYDATA, bool)

	/// Adds a handler to WM_CREATE message.
	///
	/// This handler allows the returning of a custom value.
	/// @tparam F `std::function<int(msg::wm_create)>` or `std::function<int()>`
	/// @param func: `[](msg::wm_create p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	WINLAMB_MSG_RET_TYPE(wm_create, WM_CREATE, int)

	/// Adds a handler to WM_CREATE message.
	/// @tparam F `std::function<void(msg::wm_create)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_create p) {}` or `[]() {}`
	/// @see @ref ex02
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	WINLAMB_MSG_RET_DEFAULT(wm_create, WM_CREATE)

	/// Adds a handler to WM_CTLCOLORBTN message.
	/// @tparam F `std::function<HBRUSH(msg::wm_ctl_color_btn)>` or `std::function<HBRUSH()>`
	/// @param func: `[](msg::wm_ctl_color_btn p) -> HBRUSH {}` or `[]() -> HBRUSH {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-ctlcolorbtn
	WINLAMB_MSG_RET_TYPE(wm_ctl_color_btn, WM_CTLCOLORBTN, HBRUSH)

	/// Adds a handler to WM_CTLCOLORDLG message.
	/// @tparam F `std::function<HBRUSH(msg::wm_ctl_color_dlg)>` or `std::function<HBRUSH()>`
	/// @param func: `[](msg::wm_ctl_color_dlg p) -> HBRUSH {}` or `[]() -> HBRUSH {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-ctlcolordlg
	WINLAMB_MSG_RET_TYPE(wm_ctl_color_dlg, WM_CTLCOLORDLG, HBRUSH)

	/// Adds a handler to WM_CTLCOLOREDIT message.
	/// @tparam F `std::function<HBRUSH(msg::wm_ctl_color_edit)>` or `std::function<HBRUSH()>`
	/// @param func: `[](msg::wm_ctl_color_edit p) -> HBRUSH {}` or `[]() -> HBRUSH {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-ctlcoloredit
	WINLAMB_MSG_RET_TYPE(wm_ctl_color_edit, WM_CTLCOLOREDIT, HBRUSH)

	/// Adds a handler to WM_CTLCOLORLISTBOX message.
	/// @tparam F `std::function<HBRUSH(msg::wm_ctl_color_list_box)>` or `std::function<HBRUSH()>`
	/// @param func: `[](msg::wm_ctl_color_list_box p) -> HBRUSH {}` or `[]() -> HBRUSH {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-ctlcolorlistbox
	WINLAMB_MSG_RET_TYPE(wm_ctl_color_list_box, WM_CTLCOLORLISTBOX, HBRUSH)

	/// Adds a handler to WM_CTLCOLORSCROLLBAR message.
	/// @tparam F `std::function<HBRUSH(msg::wm_ctl_color_scroll_bar)>` or `std::function<HBRUSH()>`
	/// @param func: `[](msg::wm_ctl_color_scroll_bar p) -> HBRUSH {}` or `[]() -> HBRUSH {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-ctlcolorscrollbar
	WINLAMB_MSG_RET_TYPE(wm_ctl_color_scroll_bar, WM_CTLCOLORSCROLLBAR, HBRUSH)

	/// Adds a handler to WM_CTLCOLORSTATIC message.
	/// @tparam F `std::function<HBRUSH(msg::wm_ctl_color_static)>` or `std::function<HBRUSH()>`
	/// @param func: `[](msg::wm_ctl_color_static p) -> HBRUSH {}` or `[]() -> HBRUSH {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-ctlcolorstatic
	WINLAMB_MSG_RET_TYPE(wm_ctl_color_static, WM_CTLCOLORSTATIC, HBRUSH)

	/// Adds a handler to WM_DEADCHAR message.
	/// @tparam F `std::function<void(msg::wm_dead_char)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_dead_char p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-deadchar
	WINLAMB_MSG_RET_DEFAULT(wm_dead_char, WM_DEADCHAR)

	/// Adds a handler to WM_DELETEITEM message.
	/// @tparam F `std::function<void(msg::wm_delete_item)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_delete_item p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-deleteitem
	WINLAMB_MSG_RET_VALUE(wm_delete_item, WM_DELETEITEM, TRUE)

	/// Adds a handler to WM_DESTROY message.
	/// @tparam F `std::function<void(msg::wm_destroy)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_destroy p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-destroy
	WINLAMB_MSG_RET_DEFAULT(wm_destroy, WM_DESTROY)

	/// Adds a handler to WM_DESTROYCLIPBOARD message.
	/// @tparam F `std::function<void(msg::wm_destroy_clipboard)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_destroy_clipboard p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-destroyclipboard
	WINLAMB_MSG_RET_DEFAULT(wm_destroy_clipboard, WM_DESTROYCLIPBOARD)

	/// Adds a handler to WM_DEVMODECHANGE message.
	/// @tparam F `std::function<void(msg::wm_dev_mode_change)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_dev_mode_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-devmodechange
	WINLAMB_MSG_RET_DEFAULT(wm_dev_mode_change, WM_DEVMODECHANGE)

#ifdef _DBT_H // Ras.h
	/// Adds a handler to WM_DEVICECHANGE message. Requires Ras.h.
	/// @tparam F `std::function<WORD(msg::wm_device_change)>` or `std::function<WORD()>`
	/// @param func: `[](msg::wm_device_change p) -> WORD {}` or `[]() -> WORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/devio/wm-devicechange
	WINLAMB_MSG_RET_TYPE(wm_device_change, WM_DEVICECHANGE, WORD)
#endif

	/// Adds a handler to WM_DISPLAYCHANGE message.
	/// @tparam F `std::function<void(msg::wm_display_change)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_display_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-displaychange
	WINLAMB_MSG_RET_DEFAULT(wm_display_change, WM_DISPLAYCHANGE)

	/// Adds a handler to WM_DRAWCLIPBOARD message.
	/// @tparam F `std::function<void(msg::wm_draw_clipboard)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_draw_clipboard p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-drawclipboard
	WINLAMB_MSG_RET_DEFAULT(wm_draw_clipboard, WM_DRAWCLIPBOARD)

	/// Adds a handler to WM_DRAWITEM message.
	/// @tparam F `std::function<void(msg::wm_draw_item)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_draw_item p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-drawitem
	WINLAMB_MSG_RET_VALUE(wm_draw_item, WM_DRAWITEM, TRUE)

	/// Adds a handler to WM_DROPFILES message.
	/// @tparam F `std::function<void(msg::wm_drop_files)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_drop_files p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/shell/wm-dropfiles
	WINLAMB_MSG_RET_DEFAULT(wm_drop_files, WM_DROPFILES)

	/// Adds a handler to WM_ENABLE message.
	/// @tparam F `std::function<void(msg::wm_enable)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_enable p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-enable
	WINLAMB_MSG_RET_DEFAULT(wm_enable, WM_ENABLE)

	/// Adds a handler to WM_ENDSESSION message.
	/// @tparam F `std::function<void(msg::wm_end_session)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_end_session p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/shutdown/wm-endsession
	WINLAMB_MSG_RET_DEFAULT(wm_end_session, WM_ENDSESSION)

	/// Adds a handler to WM_ENTERIDLE message.
	/// @tparam F `std::function<void(msg::wm_enter_idle)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_enter_idle p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-enteridle
	WINLAMB_MSG_RET_DEFAULT(wm_enter_idle, WM_ENTERIDLE)

	/// Adds a handler to WM_ENTERMENULOOP message.
	/// @tparam F `std::function<void(msg::wm_enter_menu_loop)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_enter_menu_loop p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-entermenuloop
	WINLAMB_MSG_RET_DEFAULT(wm_enter_menu_loop, WM_ENTERMENULOOP)

	/// Adds a handler to WM_ENTERSIZEMOVE message.
	/// @tparam F `std::function<void(msg::wm_enter_size_move)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_enter_size_move p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-entersizemove
	WINLAMB_MSG_RET_DEFAULT(wm_enter_size_move, WM_ENTERSIZEMOVE)

	/// Adds a handler to WM_ERASEBKGND message.
	/// @tparam F `std::function<int(msg::wm_erase_bkgnd)>` or `std::function<int()>`
	/// @param func: `[](msg::wm_erase_bkgnd p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-erasebkgnd
	WINLAMB_MSG_RET_TYPE(wm_erase_bkgnd, WM_ERASEBKGND, int)

	/// Adds a handler to WM_EXITMENULOOP message.
	/// @tparam F `std::function<void(msg::wm_exit_menu_loop)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_exit_menu_loop p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-exitmenuloop
	WINLAMB_MSG_RET_DEFAULT(wm_exit_menu_loop, WM_EXITMENULOOP)

	/// Adds a handler to WM_EXITSIZEMOVE message.
	/// @tparam F `std::function<void(msg::wm_exit_size_move)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_exit_size_move p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-exitsizemove
	WINLAMB_MSG_RET_DEFAULT(wm_exit_size_move, WM_EXITSIZEMOVE)

	/// Adds a handler to WM_FONTCHANGE message.
	/// @tparam F `std::function<void(msg::wm_font_change)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_font_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-fontchange
	WINLAMB_MSG_RET_DEFAULT(wm_font_change, WM_FONTCHANGE)

	/// Adds a handler to WM_GETDLGCODE message.
	/// @tparam F `std::function<WORD(msg::wm_get_dlg_code)>` or `std::function<WORD()>`
	/// @param func: `[](msg::wm_get_dlg_code p) -> WORD {}` or `[]() -> WORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-getdlgcode
	WINLAMB_MSG_RET_TYPE(wm_get_dlg_code, WM_GETDLGCODE, WORD)

	/// Adds a handler to WM_GETFONT message.
	/// @tparam F `std::function<HFONT(msg::wm_get_font)>` or `std::function<HFONT()>`
	/// @param func: `[](msg::wm_get_font p) -> HFONT {}` or `[]() -> HFONT {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-getfont
	WINLAMB_MSG_RET_TYPE(wm_get_font, WM_GETFONT, HFONT)

	/// Adds a handler to WM_GETHOTKEY message.
	/// @tparam F `std::function<WORD(msg::wm_get_hot_key)>` or `std::function<WORD()>`
	/// @param func: `[](msg::wm_get_hot_key p) -> WORD {}` or `[]() -> WORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-getfont
	WINLAMB_MSG_RET_TYPE(wm_get_hot_key, WM_GETHOTKEY, WORD)

	/// Adds a handler to WM_GETICON message.
	/// @tparam F `std::function<HICON(msg::wm_get_icon)>` or `std::function<HICON()>`
	/// @param func: `[](msg::wm_get_icon p) -> HICON {}` or `[]() -> HICON {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-geticon
	WINLAMB_MSG_RET_TYPE(wm_get_icon, WM_GETICON, HICON)

	/// Adds a handler to WM_GETMINMAXINFO message.
	/// @tparam F `std::function<void(msg::wm_get_min_max_info)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_get_min_max_info p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-getminmaxinfo
	WINLAMB_MSG_RET_DEFAULT(wm_get_min_max_info, WM_GETMINMAXINFO)

	/// Adds a handler to WM_GETTEXT message.
	/// @tparam F `std::function<UINT(msg::wm_get_text)>` or `std::function<UINT()>`
	/// @param func: `[](msg::wm_get_text p) -> UINT {}` or `[]() -> UINT {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-gettext
	WINLAMB_MSG_RET_TYPE(wm_get_text, WM_GETTEXT, UINT)

	/// Adds a handler to WM_GETTEXTLENGTH message.
	/// @tparam F `std::function<UINT(msg::wm_get_text_length)>` or `std::function<UINT()>`
	/// @param func: `[](msg::wm_get_text_length p) -> UINT {}` or `[]() -> UINT {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-gettextlength
	WINLAMB_MSG_RET_TYPE(wm_get_text_length, WM_GETTEXTLENGTH, UINT)

	/// Adds a handler to WM_GETTITLEBARINFOEX message.
	/// @tparam F `std::function<void(msg::wm_get_title_bar_info_ex)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_get_title_bar_info_ex p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-gettitlebarinfoex
	WINLAMB_MSG_RET_DEFAULT(wm_get_title_bar_info_ex, WM_GETTITLEBARINFOEX)

	/// Adds a handler to WM_HSCROLL message.
	/// @tparam F `std::function<void(msg::wm_h_scroll)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_h_scroll p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-hscroll
	WINLAMB_MSG_RET_DEFAULT(wm_h_scroll, WM_HSCROLL)

	/// Adds a handler to WM_HSCROLLCLIPBOARD message.
	/// @tparam F `std::function<void(msg::wm_h_scroll_clipboard)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_h_scroll_clipboard p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-hscrollclipboard
	WINLAMB_MSG_RET_DEFAULT(wm_h_scroll_clipboard, WM_HSCROLLCLIPBOARD)

	/// Adds a handler to WM_HELP message.
	/// @tparam F `std::function<void(msg::wm_help)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_help p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/shell/wm-help
	WINLAMB_MSG_RET_VALUE(wm_help, WM_HELP, TRUE)

	/// Adds a handler to WM_HOTKEY message.
	/// @tparam F `std::function<void(msg::wm_hot_key)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_hot_key p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-hotkey
	WINLAMB_MSG_RET_DEFAULT(wm_hot_key, WM_HOTKEY)

	/// Adds a handler to WM_INITDIALOG message.
	///
	/// This handler allows the returning of a custom value.
	/// @tparam F `std::function<bool(msg::wm_init_dialog)>` or `std::function<bool()>`
	/// @param func: `[](msg::wm_init_dialog p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	WINLAMB_MSG_RET_TYPE(wm_init_dialog, WM_INITDIALOG, bool)

	/// Adds a handler to WM_INITDIALOG message.
	/// @tparam F `std::function<void(msg::wm_init_dialog)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_init_dialog p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-initdialog
	WINLAMB_MSG_RET_DEFAULT(wm_init_dialog, WM_INITDIALOG)

	/// Adds a handler to WM_INITMENU message.
	/// @tparam F `std::function<void(msg::wm_init_menu)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_init_menu p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-initmenu
	WINLAMB_MSG_RET_DEFAULT(wm_init_menu, WM_INITMENU)

	/// Adds a handler to WM_INITMENUPOPUP message.
	/// @tparam F `std::function<void(msg::wm_init_menu_popup)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_init_menu_popup p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-initmenupopup
	WINLAMB_MSG_RET_DEFAULT(wm_init_menu_popup, WM_INITMENUPOPUP)

	/// Adds a handler to WM_INPUTLANGCHANGE message.
	/// @tparam F `std::function<void(msg::wm_input_lang_change)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_input_lang_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-inputlangchange
	WINLAMB_MSG_RET_VALUE(wm_input_lang_change, WM_INPUTLANGCHANGE, 1)

	/// Adds a handler to WM_INPUTLANGCHANGEREQUEST message.
	/// @tparam F `std::function<void(msg::wm_input_lang_change_request)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_input_lang_change_request p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-inputlangchangerequest
	WINLAMB_MSG_RET_DEFAULT(wm_input_lang_change_request, WM_INPUTLANGCHANGEREQUEST)

	/// Adds a handler to WM_KEYDOWN message.
	/// @tparam F `std::function<void(msg::wm_key_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_key_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
	WINLAMB_MSG_RET_DEFAULT(wm_key_down, WM_KEYDOWN)

	/// Adds a handler to WM_KEYUP message.
	/// @tparam F `std::function<void(msg::wm_key_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_key_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keyup
	WINLAMB_MSG_RET_DEFAULT(wm_key_up, WM_KEYUP)

	/// Adds a handler to WM_KILLFOCUS message.
	/// @tparam F `std::function<void(msg::wm_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-killfocus
	WINLAMB_MSG_RET_DEFAULT(wm_kill_focus, WM_KILLFOCUS)

	/// Adds a handler to WM_LBUTTONDBLCLK message.
	/// @tparam F `std::function<void(msg::wm_l_button_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_l_button_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttondblclk
	WINLAMB_MSG_RET_DEFAULT(wm_l_button_dbl_clk, WM_LBUTTONDBLCLK)

	/// Adds a handler to WM_LBUTTONDOWN message.
	/// @tparam F `std::function<void(msg::wm_l_button_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_l_button_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttondown
	WINLAMB_MSG_RET_DEFAULT(wm_l_button_down, WM_LBUTTONDOWN)

	/// Adds a handler to WM_LBUTTONUP message.
	/// @tparam F `std::function<void(msg::wm_l_button_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_l_button_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttonup
	WINLAMB_MSG_RET_DEFAULT(wm_l_button_up, WM_LBUTTONUP)

	/// Adds a handler to WM_MBUTTONDBLCLK message.
	/// @tparam F `std::function<void(msg::wm_m_button_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_m_button_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mbuttondblclk
	WINLAMB_MSG_RET_DEFAULT(wm_m_button_dbl_clk, WM_MBUTTONDBLCLK)

	/// Adds a handler to WM_MBUTTONDOWN message.
	/// @tparam F `std::function<void(msg::wm_m_button_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_m_button_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mbuttondown
	WINLAMB_MSG_RET_DEFAULT(wm_m_button_down, WM_MBUTTONDOWN)

	/// Adds a handler to WM_MBUTTONUP message.
	/// @tparam F `std::function<void(msg::wm_m_button_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_m_button_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mbuttonup
	WINLAMB_MSG_RET_DEFAULT(wm_m_button_up, WM_MBUTTONUP)

	/// Adds a handler to WM_MDIACTIVATE message.
	/// @tparam F `std::function<void(msg::wm_mdi_activate)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_mdi_activate p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-mdiactivate
	WINLAMB_MSG_RET_DEFAULT(wm_mdi_activate, WM_MDIACTIVATE)

	/// Adds a handler to WM_MEASUREITEM message.
	/// @tparam F `std::function<void(msg::wm_measure_item)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_measure_item p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-measureitem
	WINLAMB_MSG_RET_DEFAULT(wm_measure_item, WM_MEASUREITEM)

	/// Adds a handler to WM_MENUCHAR message.
	/// @tparam F `std::function<void(msg::wm_menu_char)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::wm_menu_char p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-menuchar
	WINLAMB_MSG_RET_TYPE(wm_menu_char, WM_MENUCHAR, DWORD)

	/// Adds a handler to WM_MENUDRAG message.
	/// @tparam F `std::function<BYTE(msg::wm_menu_drag)>` or `std::function<BYTE()>`
	/// @param func: `[](msg::wm_menu_drag p) -> BYTE {}` or `[]() -> BYTE {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-menudrag
	WINLAMB_MSG_RET_TYPE(wm_menu_drag, WM_MENUDRAG, BYTE)

	/// Adds a handler to WM_MENUGETOBJECT message.
	/// @tparam F `std::function<DWORD(msg::wm_menu_get_object)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::wm_menu_get_object p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-menugetobject
	WINLAMB_MSG_RET_TYPE(wm_menu_get_object, WM_MENUGETOBJECT, DWORD)

	/// Adds a handler to WM_MENURBUTTONUP message.
	/// @tparam F `std::function<void(msg::wm_menu_r_button_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_menu_r_button_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-menurbuttonup
	WINLAMB_MSG_RET_DEFAULT(wm_menu_r_button_up, WM_MENURBUTTONUP)

	/// Adds a handler to WM_MENUSELECT message.
	/// @tparam F `std::function<void(msg::wm_menu_select)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_menu_select p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-menuselect
	WINLAMB_MSG_RET_DEFAULT(wm_menu_select, WM_MENUSELECT)

	/// Adds a handler to WM_MOUSEACTIVATE message.
	/// @tparam F `std::function<BYTE(msg::wm_mouse_activate)>` or `std::function<BYTE()>`
	/// @param func: `[](msg::wm_mouse_activate p) -> BYTE {}` or `[]() -> BYTE {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mouseactivate
	WINLAMB_MSG_RET_TYPE(wm_mouse_activate, WM_MOUSEACTIVATE, BYTE)

	/// Adds a handler to WM_MOUSEHOVER message.
	/// @tparam F `std::function<void(msg::wm_mouse_hover)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_mouse_hover p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mousehover
	WINLAMB_MSG_RET_DEFAULT(wm_mouse_hover, WM_MOUSEHOVER)

	/// Adds a handler to WM_MOUSEHWHEEL message.
	/// @tparam F `std::function<void(msg::wm_mouse_h_wheel)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_mouse_h_wheel p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mousehwheel
	WINLAMB_MSG_RET_DEFAULT(wm_mouse_h_wheel, WM_MOUSEHWHEEL)

	/// Adds a handler to WM_MOUSELEAVE message.
	/// @tparam F `std::function<void(msg::wm_mouse_leave)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_mouse_leave p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mouseleave
	WINLAMB_MSG_RET_DEFAULT(wm_mouse_leave, WM_MOUSELEAVE)

	/// Adds a handler to WM_MOUSEMOVE message.
	/// @tparam F `std::function<void(msg::wm_mouse_move)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_mouse_move p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove
	WINLAMB_MSG_RET_DEFAULT(wm_mouse_move, WM_MOUSEMOVE)

	/// Adds a handler to WM_MOUSEWHEEL message.
	/// @tparam F `std::function<void(msg::wm_mouse_wheel)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_mouse_wheel p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel
	WINLAMB_MSG_RET_DEFAULT(wm_mouse_wheel, WM_MOUSEWHEEL)

	/// Adds a handler to WM_MOVE message.
	/// @tparam F `std::function<void(msg::wm_move)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_move p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-move
	WINLAMB_MSG_RET_DEFAULT(wm_move, WM_MOVE)

	/// Adds a handler to WM_MOVING message.
	/// @tparam F `std::function<void(msg::wm_moving)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_moving p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-moving
	WINLAMB_MSG_RET_VALUE(wm_moving, WM_MOVING, TRUE)

	/// Adds a handler to WM_NCACTIVATE message.
	/// @tparam F `std::function<bool(msg::wm_nc_activate)>` or `std::function<bool()>`
	/// @param func: `[](msg::wm_nc_activate p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-ncactivate
	WINLAMB_MSG_RET_TYPE(wm_nc_activate, WM_NCACTIVATE, bool)

	/// Adds a handler to WM_NCCALCSIZE message.
	/// @tparam F `std::function<WORD(msg::wm_nc_calc_size)>` or `std::function<WORD()>`
	/// @param func: `[](msg::wm_nc_calc_size p) -> WORD {}` or `[]() -> WORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-nccalcsize
	WINLAMB_MSG_RET_TYPE(wm_nc_calc_size, WM_NCCALCSIZE, WORD)

	/// Adds a handler to WM_NCCREATE message.
	/// @tparam F `std::function<bool(msg::wm_nc_create)>` or `std::function<bool()>`
	/// @param func: `[](msg::wm_nc_create p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-nccreate
	WINLAMB_MSG_RET_TYPE(wm_nc_create, WM_NCCREATE, bool)

	/// Adds a handler to WM_NCDESTROY message.
	///
	/// @note Default handled in:
	/// - window_main
	/// - window_modeless
	/// - dialog_main
	/// - dialog_modeless
	///
	/// @tparam F `std::function<void(msg::wm_nc_destroy)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_destroy p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-ncdestroy
	WINLAMB_MSG_RET_DEFAULT(wm_nc_destroy, WM_NCDESTROY)

	/// Adds a handler to WM_NCHITTEST message.
	/// @tparam F `std::function<int(msg::wm_nc_hit_test)>` or `std::function<int()>`
	/// @param func: `[](msg::wm_nc_hit_test p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-nchittest
	WINLAMB_MSG_RET_TYPE(wm_nc_hit_test, WM_NCHITTEST, int)

	/// Adds a handler to WM_NCLBUTTONDBLCLK message.
	/// @tparam F `std::function<void(msg::wm_nc_l_button_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_l_button_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-nclbuttondblclk
	WINLAMB_MSG_RET_DEFAULT(wm_nc_l_button_dbl_clk, WM_NCLBUTTONDBLCLK)

	/// Adds a handler to WM_NCLBUTTONDOWN message.
	/// @tparam F `std::function<void(msg::wm_nc_l_button_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_l_button_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-nclbuttondown
	WINLAMB_MSG_RET_DEFAULT(wm_nc_l_button_down, WM_NCLBUTTONDOWN)

	/// Adds a handler to WM_NCLBUTTONUP message.
	/// @tparam F `std::function<void(msg::wm_nc_l_button_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_l_button_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-nclbuttonup
	WINLAMB_MSG_RET_DEFAULT(wm_nc_l_button_up, WM_NCLBUTTONUP)

	/// Adds a handler to WM_NCMBUTTONDBLCLK message.
	/// @tparam F `std::function<void(msg::wm_nc_m_button_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_m_button_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncmbuttondblclk
	WINLAMB_MSG_RET_DEFAULT(wm_nc_m_button_dbl_clk, WM_NCMBUTTONDBLCLK)

	/// Adds a handler to WM_NCMBUTTONDOWN message.
	/// @tparam F `std::function<void(msg::wm_nc_m_button_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_m_button_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncmbuttondown
	WINLAMB_MSG_RET_DEFAULT(wm_nc_m_button_down, WM_NCMBUTTONDOWN)

	/// Adds a handler to WM_NCMBUTTONUP message.
	/// @tparam F `std::function<void(msg::wm_nc_m_button_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_m_button_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncmbuttonup
	WINLAMB_MSG_RET_DEFAULT(wm_nc_m_button_up, WM_NCMBUTTONUP)

	/// Adds a handler to WM_NCMOUSEHOVER message.
	/// @tparam F `std::function<void(msg::wm_nc_mouse_hover)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_mouse_hover p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncmousehover
	WINLAMB_MSG_RET_DEFAULT(wm_nc_mouse_hover, WM_NCMOUSEHOVER)

	/// Adds a handler to WM_NCMOUSELEAVE message.
	/// @tparam F `std::function<void(msg::wm_nc_mouse_leave)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_mouse_leave p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncmouseleave
	WINLAMB_MSG_RET_DEFAULT(wm_nc_mouse_leave, WM_NCMOUSELEAVE)

	/// Adds a handler to WM_NCMOUSEMOVE message.
	/// @tparam F `std::function<void(msg::wm_nc_mouse_move)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_mouse_move p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncmousemove
	WINLAMB_MSG_RET_DEFAULT(wm_nc_mouse_move, WM_NCMOUSEMOVE)

	/// Adds a handler to WM_NCPAINT message.
	///
	/// @note Default handled in:
	/// - window_control
	/// - dialog_control
	///
	/// @tparam F `std::function<void(msg::wm_nc_paint)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_paint p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-ncpaint
	WINLAMB_MSG_RET_DEFAULT(wm_nc_paint, WM_NCPAINT)

	/// Adds a handler to WM_NCRBUTTONDBLCLK message.
	/// @tparam F `std::function<void(msg::wm_nc_r_button_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_r_button_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncrbuttondblclk
	WINLAMB_MSG_RET_DEFAULT(wm_nc_r_button_dbl_clk, WM_NCRBUTTONDBLCLK)

	/// Adds a handler to WM_NCRBUTTONDOWN message.
	/// @tparam F `std::function<void(msg::wm_nc_r_button_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_r_button_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncrbuttondown
	WINLAMB_MSG_RET_DEFAULT(wm_nc_r_button_down, WM_NCRBUTTONDOWN)

	/// Adds a handler to WM_NCRBUTTONUP message.
	/// @tparam F `std::function<void(msg::wm_nc_r_button_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_r_button_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncrbuttonup
	WINLAMB_MSG_RET_DEFAULT(wm_nc_r_button_up, WM_NCRBUTTONUP)

	/// Adds a handler to WM_NCXBUTTONDBLCLK message.
	/// @tparam F `std::function<void(msg::wm_nc_x_button_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_x_button_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncxbuttondblclk
	WINLAMB_MSG_RET_VALUE(wm_nc_x_button_dbl_clk, WM_NCXBUTTONDBLCLK, TRUE)

	/// Adds a handler to WM_NCXBUTTONDOWN message.
	/// @tparam F `std::function<void(msg::wm_nc_x_button_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_x_button_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncxbuttondown
	WINLAMB_MSG_RET_VALUE(wm_nc_x_button_down, WM_NCXBUTTONDOWN, TRUE)

	/// Adds a handler to WM_NCXBUTTONUP message.
	/// @tparam F `std::function<void(msg::wm_nc_x_button_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_nc_x_button_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-ncxbuttonup
	WINLAMB_MSG_RET_VALUE(wm_nc_x_button_up, WM_NCXBUTTONUP, TRUE)

	/// Adds a handler to WM_NEXTDLGCTL message.
	/// @tparam F `std::function<void(msg::wm_next_dlg_ctl)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_next_dlg_ctl p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dlgbox/wm-nextdlgctl
	WINLAMB_MSG_RET_DEFAULT(wm_next_dlg_ctl, WM_NEXTDLGCTL)

	/// Adds a handler to WM_NEXTMENU message.
	/// @tparam F `std::function<void(msg::wm_next_menu)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_next_menu p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-nextmenu
	WINLAMB_MSG_RET_DEFAULT(wm_next_menu, WM_NEXTMENU)

	/// Adds a handler to WM_NOTIFYFORMAT message.
	/// @tparam F `std::function<BYTE(msg::wm_notify_format)>` or `std::function<BYTE()>`
	/// @param func: `[](msg::wm_notify_format p) -> BYTE {}` or `[]() -> BYTE {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-notifyformat
	WINLAMB_MSG_RET_TYPE(wm_notify_format, WM_NOTIFYFORMAT, BYTE)

	/// Adds a handler to WM_PAINT message.
	/// @tparam F `std::function<void(msg::wm_paint)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_paint p) {}` or `[]() {}`
	/// @see @ref ex08
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-paint
	WINLAMB_MSG_RET_DEFAULT(wm_paint, WM_PAINT)

	/// Adds a handler to WM_PAINTCLIPBOARD message.
	/// @tparam F `std::function<void(msg::wm_paint_clipboard)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_paint_clipboard p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-paintclipboard
	WINLAMB_MSG_RET_DEFAULT(wm_paint_clipboard, WM_PAINTCLIPBOARD)

	/// Adds a handler to WM_PALETTECHANGED message.
	/// @tparam F `std::function<void(msg::wm_palette_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_palette_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-palettechanged
	WINLAMB_MSG_RET_DEFAULT(wm_palette_changed, WM_PALETTECHANGED)

	/// Adds a handler to WM_PALETTEISCHANGING message.
	/// @tparam F `std::function<void(msg::wm_palette_is_changing)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_palette_is_changing p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-paletteischanging
	WINLAMB_MSG_RET_DEFAULT(wm_palette_is_changing, WM_PALETTEISCHANGING)

	/// Adds a handler to WM_PARENTNOTIFY message.
	/// @tparam F `std::function<void(msg::wm_parent_notify)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_parent_notify p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputmsg/wm-parentnotify
	WINLAMB_MSG_RET_DEFAULT(wm_parent_notify, WM_PARENTNOTIFY)

	/// Adds a handler to WM_POWERBROADCAST message.
	/// @tparam F `std::function<void(msg::wm_power_broadcast)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_power_broadcast p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/power/wm-powerbroadcast
	WINLAMB_MSG_RET_VALUE(wm_power_broadcast, WM_POWERBROADCAST, TRUE)

	/// Adds a handler to WM_PRINT message.
	/// @tparam F `std::function<void(msg::wm_print)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_print p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-print
	WINLAMB_MSG_RET_DEFAULT(wm_print, WM_PRINT)

	/// Adds a handler to WM_PRINTCLIENT message.
	/// @tparam F `std::function<void(msg::wm_print_client)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_print_client p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-printclient
	WINLAMB_MSG_RET_DEFAULT(wm_print_client, WM_PRINTCLIENT)

	/// Adds a handler to WM_QUERYDRAGICON message.
	/// @tparam F `std::function<HICON(msg::wm_query_drag_icon)>` or `std::function<HICON()>`
	/// @param func: `[](msg::wm_query_drag_icon p) -> HICON {}` or `[]() -> HICON {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-querydragicon
	WINLAMB_MSG_RET_TYPE(wm_query_drag_icon, WM_QUERYDRAGICON, HICON)

	/// Adds a handler to WM_QUERYENDSESSION message.
	/// @tparam F `std::function<bool(msg::wm_query_end_session)>` or `std::function<bool()>`
	/// @param func: `[](msg::wm_query_end_session p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/shutdown/wm-queryendsession
	WINLAMB_MSG_RET_TYPE(wm_query_end_session, WM_QUERYENDSESSION, bool)

	/// Adds a handler to WM_QUERYNEWPALETTE message.
	/// @tparam F `std::function<bool(msg::wm_query_new_palette)>` or `std::function<bool()>`
	/// @param func: `[](msg::wm_query_new_palette p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-querynewpalette
	WINLAMB_MSG_RET_TYPE(wm_query_new_palette, WM_QUERYNEWPALETTE, bool)

	/// Adds a handler to WM_QUERYOPEN message.
	/// @tparam F `std::function<bool(msg::wm_query_open)>` or `std::function<bool()>`
	/// @param func: `[](msg::wm_query_open p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-queryopen
	WINLAMB_MSG_RET_TYPE(wm_query_open, WM_QUERYOPEN, bool)

	/// Adds a handler to WM_RBUTTONDBLCLK message.
	/// @tparam F `std::function<void(msg::wm_r_button_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_r_button_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-rbuttondblclk
	WINLAMB_MSG_RET_DEFAULT(wm_r_button_dbl_clk, WM_RBUTTONDBLCLK)

	/// Adds a handler to WM_RBUTTONDOWN message.
	/// @tparam F `std::function<void(msg::wm_r_button_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_r_button_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-rbuttondown
	WINLAMB_MSG_RET_DEFAULT(wm_r_button_down, WM_RBUTTONDOWN)

	/// Adds a handler to WM_RBUTTONUP message.
	/// @tparam F `std::function<void(msg::wm_r_button_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_r_button_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-rbuttonup
	WINLAMB_MSG_RET_DEFAULT(wm_r_button_up, WM_RBUTTONUP)

#ifdef _RAS_H_ // Ras.h
	/// Adds a handler to WM_RASDIALEVENT message. Requires Ras.h.
	/// @tparam F `std::function<void(msg::wm_ras_dial_event)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_ras_dial_event p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/rras/wm-rasdialevent
	WINLAMB_MSG_RET_VALUE(wm_ras_dial_event, WM_RASDIALEVENT, TRUE)
#endif

	/// Adds a handler to WM_RENDERALLFORMATS message.
	/// @tparam F `std::function<void(msg::wm_render_all_formats)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_render_all_formats p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-renderallformats
	WINLAMB_MSG_RET_DEFAULT(wm_render_all_formats, WM_RENDERALLFORMATS)

	/// Adds a handler to WM_RENDERFORMAT message.
	/// @tparam F `std::function<void(msg::wm_render_format)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_render_format p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-renderformat
	WINLAMB_MSG_RET_DEFAULT(wm_render_format, WM_RENDERFORMAT)

	/// Adds a handler to WM_SETCURSOR message.
	/// @tparam F `std::function<void(msg::wm_set_cursor)>` or `std::function<bool()>`
	/// @param func: `[](msg::wm_set_cursor p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-setcursor
	WINLAMB_MSG_RET_TYPE(wm_set_cursor, WM_SETCURSOR, bool)

	/// Adds a handler to WM_SETFOCUS message.
	///
	/// @note Default handled in:
	/// - window_main
	/// - window_modal
	///
	/// @tparam F `std::function<void(msg::wm_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-setfocus
	WINLAMB_MSG_RET_DEFAULT(wm_set_focus, WM_SETFOCUS)

	/// Adds a handler to WM_SETFONT message.
	/// @tparam F `std::function<void(msg::wm_set_font)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_set_font p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-setfont
	WINLAMB_MSG_RET_DEFAULT(wm_set_font, WM_SETFONT)

	/// Adds a handler to WM_SETHOTKEY message.
	/// @tparam F `std::function<int(msg::wm_set_hot_key)>` or `std::function<int()>`
	/// @param func: `[](msg::wm_set_hot_key p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-sethotkey
	WINLAMB_MSG_RET_TYPE(wm_set_hot_key, WM_SETHOTKEY, int)

	/// Adds a handler to WM_SETICON message.
	/// @tparam F `std::function<HICON(msg::wm_set_icon)>` or `std::function<HICON()>`
	/// @param func: `[](msg::wm_set_icon p) -> HICON {}` or `[]() -> HICON {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-seticon
	WINLAMB_MSG_RET_TYPE(wm_set_icon, WM_SETICON, HICON)

	/// Adds a handler to WM_SETREDRAW message.
	/// @tparam F `std::function<void(msg::wm_set_redraw)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_set_redraw p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-setredraw
	WINLAMB_MSG_RET_DEFAULT(wm_set_redraw, WM_SETREDRAW)

	/// Adds a handler to WM_SETTEXT message.
	/// @tparam F `std::function<int(msg::wm_set_text)>` or `std::function<int()>`
	/// @param func: `[](msg::wm_set_text p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-settext
	WINLAMB_MSG_RET_TYPE(wm_set_text, WM_SETTEXT, int)

	/// Adds a handler to WM_SETTINGCHANGE message.
	/// @tparam F `std::function<void(msg::wm_setting_change)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_setting_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-settingchange
	WINLAMB_MSG_RET_DEFAULT(wm_setting_change, WM_SETTINGCHANGE)

	/// Adds a handler to WM_SHOWWINDOW message.
	/// @tparam F `std::function<void(msg::wm_show_window)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_show_window p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-showwindow
	WINLAMB_MSG_RET_DEFAULT(wm_show_window, WM_SHOWWINDOW)

	/// Adds a handler to WM_SIZE message.
	/// @tparam F `std::function<void(msg::wm_size)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_size p) {}` or `[]() {}`
	/// @see @ref ex04
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-size
	WINLAMB_MSG_RET_DEFAULT(wm_size, WM_SIZE)

	/// Adds a handler to WM_SIZECLIPBOARD message.
	/// @tparam F `std::function<void(msg::wm_size_clipboard)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_size_clipboard p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-sizeclipboard
	WINLAMB_MSG_RET_DEFAULT(wm_size_clipboard, WM_SIZECLIPBOARD)

	/// Adds a handler to WM_SIZING message.
	/// @tparam F `std::function<void(msg::wm_sizing)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_sizing p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-sizing
	WINLAMB_MSG_RET_VALUE(wm_sizing, WM_SIZING, TRUE)

	/// Adds a handler to WM_SPOOLERSTATUS message.
	/// @tparam F `std::function<void(msg::wm_spooler_status)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_spooler_status p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/printdocs/wm-spoolerstatus
	WINLAMB_MSG_RET_DEFAULT(wm_spooler_status, WM_SPOOLERSTATUS)

	/// Adds a handler to WM_STYLECHANGED message.
	/// @tparam F `std::function<void(msg::wm_style_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_style_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-stylechanged
	WINLAMB_MSG_RET_DEFAULT(wm_style_changed, WM_STYLECHANGED)

	/// Adds a handler to WM_STYLECHANGING message.
	/// @tparam F `std::function<void(msg::wm_style_changing)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_style_changing p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-stylechanging
	WINLAMB_MSG_RET_DEFAULT(wm_style_changing, WM_STYLECHANGING)

	/// Adds a handler to WM_SYNCPAINT message.
	/// @tparam F `std::function<void(msg::wm_sync_paint)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_sync_paint p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-syncpaint
	WINLAMB_MSG_RET_DEFAULT(wm_sync_paint, WM_SYNCPAINT)

	/// Adds a handler to WM_SYSCHAR message.
	/// @tparam F `std::function<void(msg::wm_sys_char)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_sys_char p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-syschar
	WINLAMB_MSG_RET_DEFAULT(wm_sys_char, WM_SYSCHAR)

	/// Adds a handler to WM_SYSCOLORCHANGE message.
	/// @tparam F `std::function<void(msg::wm_sys_color_change)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_sys_color_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/gdi/wm-syscolorchange
	WINLAMB_MSG_RET_DEFAULT(wm_sys_color_change, WM_SYSCOLORCHANGE)

	/// Adds a handler to WM_SYSCOMMAND message.
	/// @tparam F `std::function<void(msg::wm_sys_command)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_sys_command p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-syscommand
	WINLAMB_MSG_RET_DEFAULT(wm_sys_command, WM_SYSCOMMAND)

	/// Adds a handler to WM_SYSDEADCHAR message.
	/// @tparam F `std::function<void(msg::wm_sys_dead_char)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_sys_dead_char p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-sysdeadchar
	WINLAMB_MSG_RET_DEFAULT(wm_sys_dead_char, WM_SYSDEADCHAR)

	/// Adds a handler to WM_SYSKEYDOWN message.
	/// @tparam F `std::function<void(msg::wm_sys_key_down)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_sys_key_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown
	WINLAMB_MSG_RET_DEFAULT(wm_sys_key_down, WM_SYSKEYDOWN)

	/// Adds a handler to WM_SYSKEYUP message.
	/// @tparam F `std::function<void(msg::wm_sys_key_up)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_sys_key_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-syskeyup
	WINLAMB_MSG_RET_DEFAULT(wm_sys_key_up, WM_SYSKEYUP)

	/// Adds a handler to WM_TCARD message.
	/// @tparam F `std::function<void(msg::wm_t_card)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_t_card p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/shell/wm-tcard
	WINLAMB_MSG_RET_DEFAULT(wm_t_card, WM_TCARD)

	/// Adds a handler to WM_TIMECHANGE message.
	/// @tparam F `std::function<void(msg::wm_time_change)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_time_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/sysinfo/wm-timechange
	WINLAMB_MSG_RET_DEFAULT(wm_time_change, WM_TIMECHANGE)

	/// Adds a handler to WM_UNINITMENUPOPUP message.
	/// @tparam F `std::function<void(msg::wm_un_init_menu_popup)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_un_init_menu_popup p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/menurc/wm-uninitmenupopup
	WINLAMB_MSG_RET_DEFAULT(wm_un_init_menu_popup, WM_UNINITMENUPOPUP)

	/// Adds a handler to WM_VKEYTOITEM message.
	/// @tparam F `std::function<int(msg::wm_vkey_to_item)>` or `std::function<int()>`
	/// @param func: `[](msg::wm_vkey_to_item p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-vkeytoitem
	WINLAMB_MSG_RET_TYPE(wm_vkey_to_item, WM_VKEYTOITEM, int)

	/// Adds a handler to WM_VSCROLL message.
	/// @tparam F `std::function<void(msg::wm_v_scroll)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_v_scroll p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/wm-vscroll
	WINLAMB_MSG_RET_DEFAULT(wm_v_scroll, WM_VSCROLL)

	/// Adds a handler to WM_VSCROLLCLIPBOARD message.
	/// @tparam F `std::function<void(msg::wm_v_scroll_clipboard)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_v_scroll_clipboard p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/dataxchg/wm-vscrollclipboard
	WINLAMB_MSG_RET_DEFAULT(wm_v_scroll_clipboard, WM_VSCROLLCLIPBOARD)

	/// Adds a handler to WM_WINDOWPOSCHANGED message.
	/// @tparam F `std::function<void(msg::wm_window_pos_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_window_pos_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-windowposchanged
	WINLAMB_MSG_RET_DEFAULT(wm_window_pos_changed, WM_WINDOWPOSCHANGED)

	/// Adds a handler to WM_WINDOWPOSCHANGING message.
	/// @tparam F `std::function<void(msg::wm_window_pos_changing)>` or `std::function<void()>`
	/// @param func: `[](msg::wm_window_pos_changing p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-windowposchanging
	WINLAMB_MSG_RET_DEFAULT(wm_window_pos_changing, WM_WINDOWPOSCHANGING)
};

}//namespace wl::msg

#undef WINLAMB_MSG_RET_TYPE
#undef WINLAMB_MSG_RET_DEFAULT
#undef WINLAMB_MSG_RET_VALUE