/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <vector>
#include <Windows.h>
#include "base_native_control.h"
#include "interfaces.h"
#include "lambda_type.h"
#include "msg_wm.h"

#define WINLAMB_COMMAND_RET_DEFAULT(methodName, msgCmd) \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, void(methodName), void) \
	{ \
		this->_base.parent().on().wm_command(this->_base.ctrl_id(), msgCmd, \
			[func{std::forward<F>(func)}](wl::msg::wm_command p) { func(p); }); \
	} \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, void(), void) \
	{ \
		this->_base.parent().on().wm_command(this->_base.ctrl_id(), msgCmd, \
			[func{std::forward<F>(func)}](wl::msg::wm_command) { func(); }); \
	}

#define WINLAMB_NOTIFY_RET_DEFAULT(methodName, nmCode) \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, void(methodName), void) \
	{ \
		this->_base.parent().on().wm_notify(this->_base.ctrl_id(), nmCode, \
			[func{std::forward<F>(func)}, this](wl::msg::wm_notify p) -> LRESULT { \
				func(p); \
				return this->_base.parent_is_dlg() ? TRUE : 0; \
			}); \
	} \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, void(), void) \
	{ \
		this->_base.parent().on().wm_notify(this->_base.ctrl_id(), nmCode, \
			[func{std::forward<F>(func)}, this](wl::msg::wm_notify) -> LRESULT { \
				func(); \
				return this->_base.parent_is_dlg() ? TRUE : 0; \
			}); \
	}

#define WINLAMB_NOTIFY_RET_TYPE(methodName, nmCode, retType) \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, retType(methodName), void) \
	{ \
		this->_base.parent().on().wm_notify(this->_base.ctrl_id(), nmCode, \
			[func{std::forward<F>(func)}](wl::msg::wm_notify p) -> LRESULT { return func(p); }); \
	} \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, retType(), void) \
	{ \
		this->_base.parent().on().wm_notify(this->_base.ctrl_id(), nmCode, \
			[func{std::forward<F>(func)}, this](wl::msg::wm_notify) -> LRESULT { return func(); }); \
	}

namespace wl::msg::button {

/// Exposes handler methods to wl::button notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to BCN_DROPDOWN wl::button notification.
	/// @tparam F `std::function<void(msg::button::bcn_drop_down)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bcn_drop_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bcn-dropdown
	WINLAMB_NOTIFY_RET_DEFAULT(bcn_drop_down, BCN_DROPDOWN)

	/// Adds a handler to BCN_HOTITEMCHANGE wl::button notification.
	/// @tparam F `std::function<void(msg::button::bcn_hot_item_change)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bcn_hot_item_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bcn-hotitemchange
	WINLAMB_NOTIFY_RET_DEFAULT(bcn_hot_item_change, BCN_HOTITEMCHANGE)

	/// Adds a handler to BN_CLICKED wl::button notification.
	/// @tparam F `std::function<void(msg::button::bn_clicked)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bn_clicked p) {}` or `[]() {}`
	/// @see @ref ex02
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bn-clicked
	WINLAMB_COMMAND_RET_DEFAULT(bn_clicked, BN_CLICKED)

	/// Adds a handler to BN_DBLCLK wl::button notification.
	/// @tparam F `std::function<void(msg::button::bn_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bn_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bn-dblclk
	WINLAMB_COMMAND_RET_DEFAULT(bn_dbl_clk, BN_DBLCLK)

	/// Adds a handler to BN_KILLFOCUS wl::button notification.
	/// @tparam F `std::function<void(msg::button::bn_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bn_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bn-killfocus
	WINLAMB_COMMAND_RET_DEFAULT(bn_kill_focus, BN_KILLFOCUS)

	/// Adds a handler to BN_SETFOCUS wl::button notification.
	/// @tparam F `std::function<void(msg::button::bn_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bn_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bn-setfocus
	WINLAMB_COMMAND_RET_DEFAULT(bn_set_focus, BN_SETFOCUS)

	/// Adds a handler to NM_CUSTOMDRAW wl::button notification.
	/// @tparam F `std::function<DWORD(msg::button::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::button::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-button
	WINLAMB_NOTIFY_RET_TYPE(nm_custom_draw, NM_CUSTOMDRAW, DWORD)
};

}//namespace wl::msg::button

namespace wl::msg::combo_box {

/// Exposes handler methods to wl::combo_box notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to CBN_CLOSEUP wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_close_up)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_close_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-closeup
	WINLAMB_COMMAND_RET_DEFAULT(cbn_close_up, CBN_CLOSEUP)

	/// Adds a handler to CBN_DBLCLK wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-dblclk
	WINLAMB_COMMAND_RET_DEFAULT(cbn_dbl_clk, CBN_DBLCLK)

	/// Adds a handler to CBN_DROPDOWN wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_drop_down)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_drop_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-dropdown
	WINLAMB_COMMAND_RET_DEFAULT(cbn_drop_down, CBN_DROPDOWN)

	/// Adds a handler to CBN_EDITCHANGE wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_edit_change)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_edit_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-editchange
	WINLAMB_COMMAND_RET_DEFAULT(cbn_edit_change, CBN_EDITCHANGE)

	/// Adds a handler to CBN_EDITUPDATE wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_edit_update)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_edit_update p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-editupdate
	WINLAMB_COMMAND_RET_DEFAULT(cbn_edit_update, CBN_EDITUPDATE)

	/// Adds a handler to CBN_ERRSPACE wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_err_space)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_err_space p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-errspace
	WINLAMB_COMMAND_RET_DEFAULT(cbn_err_space, CBN_ERRSPACE)

	/// Adds a handler to CBN_KILLFOCUS wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-killfocus
	WINLAMB_COMMAND_RET_DEFAULT(cbn_kill_focus, CBN_KILLFOCUS)

	/// Adds a handler to CBN_SELCHANGE wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_sel_change)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_sel_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-selchange
	WINLAMB_COMMAND_RET_DEFAULT(cbn_sel_change, CBN_SELCHANGE)

	/// Adds a handler to CBN_SELENDCANCEL wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_sel_end_cancel)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_sel_end_cancel p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-selendcancel
	WINLAMB_COMMAND_RET_DEFAULT(cbn_sel_end_cancel, CBN_SELENDCANCEL)

	/// Adds a handler to CBN_SELENDOK wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_sel_end_ok)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_sel_end_ok p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-selendok
	WINLAMB_COMMAND_RET_DEFAULT(cbn_sel_end_ok, CBN_SELENDOK)

	/// Adds a handler to CBN_SETFOCUS wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-setfocus
	WINLAMB_COMMAND_RET_DEFAULT(cbn_set_focus, CBN_SETFOCUS)
};

}//namespace wl::msg::combo_box

namespace wl::msg::combo_box_ex {

/// Exposes handler methods to wl::combo_box_ex notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	// Copied from combo_box.
	// We can't simply inherit because we would have to use combo_box argument
	// types instead of combo_box_ex ones.

	/// Adds a handler to CBN_CLOSEUP wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_close_up)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_close_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-closeup
	WINLAMB_COMMAND_RET_DEFAULT(cbn_close_up, CBN_CLOSEUP)

	/// Adds a handler to CBN_DBLCLK wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-dblclk
	WINLAMB_COMMAND_RET_DEFAULT(cbn_dbl_clk, CBN_DBLCLK)

	/// Adds a handler to CBN_DROPDOWN wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_drop_down)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_drop_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-dropdown
	WINLAMB_COMMAND_RET_DEFAULT(cbn_drop_down, CBN_DROPDOWN)

	/// Adds a handler to CBN_EDITCHANGE wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_edit_change)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_edit_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-editchange
	WINLAMB_COMMAND_RET_DEFAULT(cbn_edit_change, CBN_EDITCHANGE)

	/// Adds a handler to CBN_EDITUPDATE wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_edit_update)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_edit_update p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-editupdate
	WINLAMB_COMMAND_RET_DEFAULT(cbn_edit_update, CBN_EDITUPDATE)

	/// Adds a handler to CBN_ERRSPACE wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_err_space)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_err_space p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-errspace
	WINLAMB_COMMAND_RET_DEFAULT(cbn_err_space, CBN_ERRSPACE)

	/// Adds a handler to CBN_KILLFOCUS wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-killfocus
	WINLAMB_COMMAND_RET_DEFAULT(cbn_kill_focus, CBN_KILLFOCUS)

	/// Adds a handler to CBN_SELCHANGE wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_sel_change)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_sel_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-selchange
	WINLAMB_COMMAND_RET_DEFAULT(cbn_sel_change, CBN_SELCHANGE)

	/// Adds a handler to CBN_SELENDCANCEL wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_sel_end_cancel)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_sel_end_cancel p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-selendcancel
	WINLAMB_COMMAND_RET_DEFAULT(cbn_sel_end_cancel, CBN_SELENDCANCEL)

	/// Adds a handler to CBN_SELENDOK wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_sel_end_ok)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_sel_end_ok p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-selendok
	WINLAMB_COMMAND_RET_DEFAULT(cbn_sel_end_ok, CBN_SELENDOK)

	/// Adds a handler to CBN_SETFOCUS wl::combo_box notification.
	/// @tparam F `std::function<void(msg::combo_box::cbn_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box::cbn_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cbn-setfocus
	WINLAMB_COMMAND_RET_DEFAULT(cbn_set_focus, CBN_SETFOCUS)

	// Specific to combo_box_ex.

	/// Adds a handler to CBEN_BEGINEDIT wl::combo_box_ex notification.
	/// @tparam F `std::function<void(msg::combo_box_ex::cben_begin_edit)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box_ex::cben_begin_edit p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cben-beginedit
	WINLAMB_NOTIFY_RET_DEFAULT(cben_begin_edit, CBEN_BEGINEDIT)

	/// Adds a handler to CBEN_DELETEITEM wl::combo_box_ex notification.
	/// @tparam F `std::function<void(msg::combo_box_ex::cben_delete_item)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box_ex::cben_delete_item p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cben-deleteitem
	WINLAMB_NOTIFY_RET_DEFAULT(cben_delete_item, CBEN_DELETEITEM)

	/// Adds a handler to CBEN_DRAGBEGIN wl::combo_box_ex notification.
	/// @tparam F `std::function<void(msg::combo_box_ex::cben_drag_begin)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box_ex::cben_drag_begin p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cben-dragbegin
	WINLAMB_NOTIFY_RET_DEFAULT(cben_drag_begin, CBEN_DRAGBEGINW)

	/// Adds a handler to CBEN_ENDEDIT wl::combo_box_ex notification.
	/// @tparam F `std::function<bool(msg::combo_box_ex::cben_end_edit)>` or `std::function<bool()>`
	/// @param func: `[](msg::combo_box_ex::cben_end_edit p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cben-endedit
	WINLAMB_NOTIFY_RET_TYPE(cben_end_edit, CBEN_ENDEDITW, bool)

	/// Adds a handler to CBEN_GETDISPINFO wl::combo_box_ex notification.
	/// @tparam F `std::function<void(msg::combo_box_ex::cben_get_disp_info)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box_ex::cben_get_disp_info p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cben-getdispinfo
	WINLAMB_NOTIFY_RET_DEFAULT(cben_get_disp_info, CBEN_GETDISPINFOW)

	/// Adds a handler to CBEN_INSERTITEM wl::combo_box_ex notification.
	/// @tparam F `std::function<void(msg::combo_box_ex::cben_insert_item)>` or `std::function<void()>`
	/// @param func: `[](msg::combo_box_ex::cben_insert_item p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/cben-insertitem
	WINLAMB_NOTIFY_RET_DEFAULT(cben_insert_item, CBEN_INSERTITEM)

	/// Adds a handler to NM_SETCURSOR wl::combo_box_ex notification.
	/// @tparam F `std::function<int(msg::combo_box_ex::nm_set_cursor)>` or `std::function<int()>`
	/// @param func: `[](msg::combo_box_ex::nm_set_cursor p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-setcursor-comboboxex-
	WINLAMB_NOTIFY_RET_TYPE(nm_set_cursor, NM_SETCURSOR, int)
};

}//namespace wl::msg::combo_box_ex

namespace wl::msg::date_time_picker {

/// Exposes handler methods to wl::date_time_picker notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to DTN_CLOSEUP wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::dtn_closeup)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::dtn_close_up p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/dtn-closeup
	WINLAMB_NOTIFY_RET_DEFAULT(dtn_close_up, DTN_CLOSEUP)

	/// Adds a handler to DTN_DATETIMECHANGE wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::dtn_date_time_change)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::dtn_date_time_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/dtn-datetimechange
	WINLAMB_NOTIFY_RET_DEFAULT(dtn_date_time_change, DTN_DATETIMECHANGE)

	/// Adds a handler to DTN_DROPDOWN wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::dtn_drop_down)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::dtn_drop_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/dtn-dropdown
	WINLAMB_NOTIFY_RET_DEFAULT(dtn_drop_down, DTN_DROPDOWN)

	/// Adds a handler to DTN_FORMAT wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::dtn_format)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::dtn_format p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/dtn-format
	WINLAMB_NOTIFY_RET_DEFAULT(dtn_format, DTN_FORMATW)

	/// Adds a handler to DTN_FORMATQUERY wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::dtn_format_query)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::dtn_format_query p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/dtn-formatquery
	WINLAMB_NOTIFY_RET_DEFAULT(dtn_format_query, DTN_FORMATQUERYW)

	/// Adds a handler to DTN_USERSTRING wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::dtn_user_string)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::dtn_user_string p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/dtn-userstring
	WINLAMB_NOTIFY_RET_DEFAULT(dtn_user_string, DTN_USERSTRINGW)

	/// Adds a handler to DTN_WMKEYDOWN wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::dtn_wm_key_down)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::dtn_wm_key_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/dtn-wmkeydown
	WINLAMB_NOTIFY_RET_DEFAULT(dtn_wm_key_down, DTN_WMKEYDOWNW)

	/// Adds a handler to NM_KILLFOCUS wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::nm_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::nm_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-killfocus-date-time
	WINLAMB_NOTIFY_RET_DEFAULT(nm_kill_focus, NM_KILLFOCUS)

	/// Adds a handler to NM_SETFOCUS wl::date_time_picker notification.
	/// @tparam F `std::function<void(msg::date_time_picker::nm_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::date_time_picker::nm_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-setfocus-date-time-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_set_focus, NM_SETFOCUS)
};

}//namespace wl::msg::date_time_picker

namespace wl::msg::edit {

/// Exposes handler methods to wl::edit notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to EN_ALIGN_LTR_EC wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_align_ltr_ec)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_align_ltr_ec p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-align-ltr-ec
	WINLAMB_COMMAND_RET_DEFAULT(en_align_ltr_ec, EN_ALIGN_LTR_EC)

	/// Adds a handler to EN_ALIGN_RTL_EC wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_align_rtl_ec)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_align_rtl_ec p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-align-rtl-ec
	WINLAMB_COMMAND_RET_DEFAULT(en_align_rtl_ec, EN_ALIGN_RTL_EC)

	/// Adds a handler to EN_CHANGE wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_change)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-change
	WINLAMB_COMMAND_RET_DEFAULT(en_change, EN_CHANGE)

	/// Adds a handler to EN_ERRSPACE wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_err_space)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_err_space p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-errspace
	WINLAMB_COMMAND_RET_DEFAULT(en_err_space, EN_ERRSPACE)

	/// Adds a handler to EN_HSCROLL wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_h_scroll)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_h_scroll p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-hscroll
	WINLAMB_COMMAND_RET_DEFAULT(en_h_scroll, EN_HSCROLL)

	/// Adds a handler to EN_KILLFOCUS wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-killfocus
	WINLAMB_COMMAND_RET_DEFAULT(en_kill_focus, EN_KILLFOCUS)

	/// Adds a handler to EN_MAXTEXT wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_max_text)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_max_text p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-maxtext
	WINLAMB_COMMAND_RET_DEFAULT(en_max_text, EN_MAXTEXT)

	/// Adds a handler to EN_SETFOCUS wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-setfocus
	WINLAMB_COMMAND_RET_DEFAULT(en_set_focus, EN_SETFOCUS)

	/// Adds a handler to EN_UPDATE wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_update)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_update p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-update
	WINLAMB_COMMAND_RET_DEFAULT(en_update, EN_UPDATE)

	/// Adds a handler to EN_VSCROLL wl::edit notification.
	/// @tparam F `std::function<void(msg::edit::en_v_scroll)>` or `std::function<void()>`
	/// @param func: `[](msg::edit::en_v_scroll p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-vscroll
	WINLAMB_COMMAND_RET_DEFAULT(en_v_scroll, EN_VSCROLL)
};

}//namespace wl::msg::edit

namespace wl::msg::header {

/// Exposes handler methods to wl::header notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to HDN_BEGINDRAG wl::header notification.
	/// @tparam F `std::function<bool(msg::header::hdn_begin_drag)>` or `std::function<bool()>`
	/// @param func: `[](msg::header::hdn_begin_drag p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-begindrag
	WINLAMB_NOTIFY_RET_TYPE(hdn_begin_drag, HDN_BEGINDRAG, bool)

	/// Adds a handler to HDN_BEGINFILTEREDIT wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_begin_filter_edit)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_begin_filter_edit p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-beginfilteredit
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_begin_filter_edit, HDN_BEGINFILTEREDIT)

	/// Adds a handler to HDN_BEGINTRACK wl::header notification.
	/// @tparam F `std::function<bool(msg::header::hdn_begin_track)>` or `std::function<bool()>`
	/// @param func: `[](msg::header::hdn_begin_track p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-begintrack
	WINLAMB_NOTIFY_RET_TYPE(hdn_begin_track, HDN_BEGINTRACK, bool)

	/// Adds a handler to HDN_DIVIDERDBLCLICK wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_divider_dbl_click)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_divider_dbl_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-dividerdblclick
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_divider_dbl_click, HDN_DIVIDERDBLCLICK)

	/// Adds a handler to HDN_DROPDOWN wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_drop_down)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_drop_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-dropdown
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_drop_down, HDN_DROPDOWN)

	/// Adds a handler to HDN_ENDDRAG wl::header notification.
	/// @tparam F `std::function<bool(msg::header::hdn_end_drag)>` or `std::function<bool()>`
	/// @param func: `[](msg::header::hdn_end_drag p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-enddrag
	WINLAMB_NOTIFY_RET_TYPE(hdn_end_drag, HDN_ENDDRAG, bool)

	/// Adds a handler to HDN_ENDFILTEREDIT wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_end_filter_edit)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_end_filter_edit p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-endfilteredit
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_end_filter_edit, HDN_ENDFILTEREDIT)

	/// Adds a handler to HDN_ENDTRACK wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_end_track)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_end_track p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-endtrack
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_end_track, HDN_ENDTRACK)

	/// Adds a handler to HDN_FILTERBTNCLICK wl::header notification.
	/// @tparam F `std::function<bool(msg::header::hdn_filter_btn_click)>` or `std::function<bool()>`
	/// @param func: `[](msg::header::hdn_filter_btn_click p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-filterbtnclick
	WINLAMB_NOTIFY_RET_TYPE(hdn_filter_btn_click, HDN_FILTERBTNCLICK, bool)

	/// Adds a handler to HDN_FILTERCHANGE wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_filter_change)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_filter_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-filterchange
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_filter_change, HDN_FILTERCHANGE)

	/// Adds a handler to HDN_GETDISPINFO wl::header notification.
	/// @tparam F `std::function<LRESULT(msg::header::hdn_get_disp_info)>` or `std::function<LRESULT()>`
	/// @param func: `[](msg::header::hdn_get_disp_info p) -> LRESULT {}` or `[]() -> LRESULT {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-getdispinfo
	WINLAMB_NOTIFY_RET_TYPE(hdn_get_disp_info, HDN_GETDISPINFO, LRESULT)

	/// Adds a handler to HDN_ITEMCHANGED wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_item_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_item_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-itemchanged
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_item_changed, HDN_ITEMCHANGED)

	/// Adds a handler to HDN_ITEMCHANGING wl::header notification.
	/// @tparam F `std::function<bool(msg::header::hdn_item_changing)>` or `std::function<bool()>`
	/// @param func: `[](msg::header::hdn_item_changing p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-itemchanging
	WINLAMB_NOTIFY_RET_TYPE(hdn_item_changing, HDN_ITEMCHANGING, bool)

	/// Adds a handler to HDN_ITEMCLICK wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_item_click)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_item_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-itemclick
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_item_click, HDN_ITEMCLICK)

	/// Adds a handler to HDN_ITEMDBLCLICK wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_item_dbl_click)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_item_dbl_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-itemdblclick
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_item_dbl_click, HDN_ITEMDBLCLICK)

	/// Adds a handler to HDN_ITEMKEYDOWN wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_item_key_down)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_item_key_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-itemkeydown
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_item_key_down, HDN_ITEMKEYDOWN)

	/// Adds a handler to HDN_ITEMSTATEICONCLICK wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_item_state_icon_click)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_item_state_icon_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-itemstateiconclick
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_item_state_icon_click, HDN_ITEMSTATEICONCLICK)

	/// Adds a handler to HDN_OVERFLOWCLICK wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_overflow_click)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_overflow_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-overflowclick
	WINLAMB_NOTIFY_RET_DEFAULT(hdn_overflow_click, HDN_OVERFLOWCLICK)

	/// Adds a handler to HDN_TRACK wl::header notification.
	/// @tparam F `std::function<bool(msg::header::hdn_track)>` or `std::function<bool()>`
	/// @param func: `[](msg::header::hdn_track p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/hdn-track
	WINLAMB_NOTIFY_RET_TYPE(hdn_track, HDN_TRACK, bool)

	/// Adds a handler to NM_CUSTOMDRAW wl::header notification.
	/// @tparam F `std::function<DWORD(msg::header::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::header::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-header
	WINLAMB_NOTIFY_RET_TYPE(nm_custom_draw, NM_CUSTOMDRAW, DWORD)

	/// Adds a handler to NM_RCLICK wl::header notification.
	/// @tparam F `std::function<int(msg::header::nm_r_click)>` or `std::function<int()>`
	/// @param func: `[](msg::header::nm_r_click p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rclick-header
	WINLAMB_NOTIFY_RET_TYPE(nm_r_click, NM_RCLICK, int)

	/// Adds a handler to NM_RELEASEDCAPTURE wl::header notification.
	/// @tparam F `std::function<void(msg::header::hdn_released_capture)>` or `std::function<void()>`
	/// @param func: `[](msg::header::hdn_released_capture p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-releasedcapture-header-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_released_capture, NM_RELEASEDCAPTURE)
};

}//namespace wl::msg::header

namespace wl::msg::ip_addr {

/// Exposes handler methods to wl::ip_addr notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to IPN_FIELDCHANGED wl::ip_addr notification.
	/// @tparam F `std::function<void(msg::ip_addr::ipn_field_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::ip_addr::ipn_field_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ipn-fieldchanged
	WINLAMB_NOTIFY_RET_DEFAULT(ipn_field_changed, IPN_FIELDCHANGED)

	/// Adds a handler to EN_CHANGE wl::ip_addr notification.
	/// @tparam F `std::function<void(msg::ip_addr::en_change)>` or `std::function<void()>`
	/// @param func: `[](msg::ip_addr::en_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-change
	WINLAMB_COMMAND_RET_DEFAULT(en_change, EN_CHANGE)

	/// Adds a handler to EN_KILLFOCUS wl::ip_addr notification.
	/// @tparam F `std::function<void(msg::ip_addr::en_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::ip_addr::en_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-killfocus
	WINLAMB_COMMAND_RET_DEFAULT(en_kill_focus, EN_KILLFOCUS)

	/// Adds a handler to EN_SETFOCUS wl::ip_addr notification.
	/// @tparam F `std::function<void(msg::ip_addr::en_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::ip_addr::en_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/en-setfocus
	WINLAMB_COMMAND_RET_DEFAULT(en_set_focus, EN_SETFOCUS)
};

}//namespace wl::msg::ip_addr

namespace wl::msg::label {

/// Exposes handler methods to wl::label notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to STN_CLICKED wl::label notification.
	/// @tparam F `std::function<void(msg::label::stn_clicked)>` or `std::function<void()>`
	/// @param func: `[](msg::label::stn_clicked p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/stn-clicked
	WINLAMB_COMMAND_RET_DEFAULT(stn_clicked, STN_CLICKED)

	/// Adds a handler to STN_DBLCLK wl::label notification.
	/// @tparam F `std::function<void(msg::label::stn_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::label::stn_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/stn-dblclk
	WINLAMB_COMMAND_RET_DEFAULT(stn_dbl_clk, STN_DBLCLK)

	/// Adds a handler to STN_DISABLE wl::label notification.
	/// @tparam F `std::function<void(msg::label::stn_disable)>` or `std::function<void()>`
	/// @param func: `[](msg::label::stn_disable p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/stn-disable
	WINLAMB_COMMAND_RET_DEFAULT(stn_disable, STN_DISABLE)

	/// Adds a handler to STN_ENABLE wl::label notification.
	/// @tparam F `std::function<void(msg::label::stn_enable)>` or `std::function<void()>`
	/// @param func: `[](msg::label::stn_enable p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/stn-enable
	WINLAMB_COMMAND_RET_DEFAULT(stn_enable, STN_ENABLE)
};

}//namespace wl::msg::label

namespace wl::msg::list_box {

/// Exposes handler methods to wl::list_box notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to LBN_DBLCLK wl::list_box notification.
	/// @tparam F `std::function<void(msg::list_box::lbn_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::list_box::lbn_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lbn-dblclk
	WINLAMB_COMMAND_RET_DEFAULT(lbn_dbl_clk, LBN_DBLCLK)

	/// Adds a handler to LBN_ERRSPACE wl::list_box notification.
	/// @tparam F `std::function<void(msg::list_box::lbn_err_space)>` or `std::function<void()>`
	/// @param func: `[](msg::list_box::lbn_err_space p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lbn-errspace
	WINLAMB_COMMAND_RET_DEFAULT(lbn_err_space, LBN_ERRSPACE)

	/// Adds a handler to LBN_KILLFOCUS wl::list_box notification.
	/// @tparam F `std::function<void(msg::list_box::lbn_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::list_box::lbn_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lbn-killfocus
	WINLAMB_COMMAND_RET_DEFAULT(lbn_kill_focus, LBN_KILLFOCUS)

	/// Adds a handler to LBN_SELCANCEL wl::list_box notification.
	/// @tparam F `std::function<void(msg::list_box::lbn_sel_cancel)>` or `std::function<void()>`
	/// @param func: `[](msg::list_box::lbn_sel_cancel p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lbn-selcancel
	WINLAMB_COMMAND_RET_DEFAULT(lbn_sel_cancel, LBN_SELCANCEL)

	/// Adds a handler to LBN_SELCHANGE wl::list_box notification.
	/// @tparam F `std::function<void(msg::list_box::lbn_sel_change)>` or `std::function<void()>`
	/// @param func: `[](msg::list_box::lbn_sel_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lbn-selchange
	WINLAMB_COMMAND_RET_DEFAULT(lbn_sel_change, LBN_SELCHANGE)

	/// Adds a handler to LBN_SETFOCUS wl::list_box notification.
	/// @tparam F `std::function<void(msg::list_box::lbn_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::list_box::lbn_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lbn-setfocus
	WINLAMB_COMMAND_RET_DEFAULT(lbn_set_focus, LBN_SETFOCUS)
};

}//namespace wl::msg::list_box

namespace wl::msg::list_view {

/// Exposes handler methods to wl::list_view notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to LVN_BEGINDRAG wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_begin_drag)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_begin_drag p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-begindrag
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_begin_drag, LVN_BEGINDRAG)

	/// Adds a handler to LVN_BEGINLABELEDIT wl::list_view notification.
	/// @tparam F `std::function<bool(msg::list_view::lvn_begin_label_edit)>` or `std::function<bool()>`
	/// @param func: `[](msg::list_view::lvn_begin_label_edit p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-beginlabeledit
	WINLAMB_NOTIFY_RET_TYPE(lvn_begin_label_edit, LVN_BEGINLABELEDITW, bool)

	/// Adds a handler to LVN_BEGINRDRAG wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_begin_r_drag)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_begin_r_drag p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-beginrdrag
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_begin_r_drag, LVN_BEGINRDRAG)

	/// Adds a handler to LVN_BEGINSCROLL wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_begin_scroll)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_begin_scroll p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-beginscroll
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_begin_scroll, LVN_BEGINSCROLL)

	/// Adds a handler to LVN_COLUMNCLICK wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_column_click)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_column_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-columnclick
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_column_click, LVN_COLUMNCLICK)

	/// Adds a handler to LVN_COLUMNDROPDOWN wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_drop_down)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_column_drop_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-columndropdown
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_column_drop_down, LVN_COLUMNDROPDOWN)

	/// Adds a handler to LVN_COLUMNOVERFLOWCLICK wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_column_overflow_click)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_column_overflow_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-columnoverflowclick
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_column_overflow_click, LVN_COLUMNOVERFLOWCLICK)

	/// Adds a handler to LVN_DELETEALLITEMS wl::list_view notification.
	/// @tparam F `std::function<bool(msg::list_view::lvn_delete_all_items)>` or `std::function<bool()>`
	/// @param func: `[](msg::list_view::lvn_delete_all_items p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-deleteallitems
	WINLAMB_NOTIFY_RET_TYPE(lvn_delete_all_items, LVN_DELETEALLITEMS, bool)

	/// Adds a handler to LVN_DELETEITEM wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_delete_item)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_delete_item p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-deleteitem
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_delete_item, LVN_DELETEITEM)

	/// Adds a handler to LVN_ENDLABELEDIT wl::list_view notification.
	/// @tparam F `std::function<bool(msg::list_view::lvn_end_label_edit)>` or `std::function<bool()>`
	/// @param func: `[](msg::list_view::lvn_end_label_edit p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-endlabeledit
	WINLAMB_NOTIFY_RET_TYPE(lvn_end_label_edit, LVN_ENDLABELEDITW, bool)

	/// Adds a handler to LVN_ENDSCROLL wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_end_scroll)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_end_scroll p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-endscroll
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_end_scroll, LVN_ENDSCROLL)

	/// Adds a handler to LVN_GETDISPINFO wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_get_disp_info)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_get_disp_info p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-getdispinfo
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_get_disp_info, LVN_GETDISPINFOW)

	/// Adds a handler to LVN_GETEMPTYMARKUP wl::list_view notification.
	/// @tparam F `std::function<bool(msg::list_view::lvn_get_empty_markup)>` or `std::function<bool()>`
	/// @param func: `[](msg::list_view::lvn_get_empty_markup p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-getemptymarkup
	WINLAMB_NOTIFY_RET_TYPE(lvn_get_empty_markup, LVN_GETEMPTYMARKUP, bool)

	/// Adds a handler to LVN_GETINFOTIP wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_get_info_tip)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_get_info_tip p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-getinfotip
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_get_info_tip, LVN_GETINFOTIPW)

	/// Adds a handler to LVN_HOTTRACK wl::list_view notification.
	/// @tparam F `std::function<int(msg::list_view::lvn_hot_track)>` or `std::function<int()>`
	/// @param func: `[](msg::list_view::lvn_hot_track p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-hottrack
	WINLAMB_NOTIFY_RET_TYPE(lvn_hot_track, LVN_HOTTRACK, int)

	/// Adds a handler to LVN_INCREMENTALSEARCH wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_incremental_search)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_incremental_search p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-incrementalsearch
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_incremental_search, LVN_INCREMENTALSEARCHW)

	/// Adds a handler to LVN_INSERTITEM wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_insert_item)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_insert_item p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-insertitem
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_insert_item, LVN_INSERTITEM)

	/// Adds a handler to LVN_ITEMACTIVATE wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_item_activate)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_item_activate p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-itemactivate
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_item_activate, LVN_ITEMACTIVATE)

	/// Adds a handler to LVN_ITEMCHANGED wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_item_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_item_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-itemchanged
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_item_changed, LVN_ITEMCHANGED)

	/// Adds a handler to LVN_ITEMCHANGING wl::list_view notification.
	/// @tparam F `std::function<bool(msg::list_view::lvn_item_changing)>` or `std::function<bool()>`
	/// @param func: `[](msg::list_view::lvn_item_changing p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-itemchanging
	WINLAMB_NOTIFY_RET_TYPE(lvn_item_changing, LVN_ITEMCHANGING, bool)

	/// Adds a handler to LVN_KEYDOWN wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_key_down)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_key_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-keydown
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_key_down, LVN_KEYDOWN)

	/// Adds a handler to LVN_LINKCLICK wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_link_click)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_link_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-linkclick
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_link_click, LVN_LINKCLICK)

	/// Adds a handler to LVN_MARQUEEBEGIN wl::list_view notification.
	/// @tparam F `std::function<int(msg::list_view::lvn_marquee_begin)>` or `std::function<int()>`
	/// @param func: `[](msg::list_view::lvn_marquee_begin p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-marqueebegin
	WINLAMB_NOTIFY_RET_TYPE(lvn_marquee_begin, LVN_MARQUEEBEGIN, int)

	/// Adds a handler to LVN_ODCACHEHINT wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_od_cache_hint)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_od_cache_hint p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-odcachehint
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_od_cache_hint, LVN_ODCACHEHINT)

	/// Adds a handler to LVN_ODFINDITEM wl::list_view notification.
	/// @tparam F `std::function<int(msg::list_view::lvn_od_find_item)>` or `std::function<int()>`
	/// @param func: `[](msg::list_view::lvn_od_find_item p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-odfinditem
	WINLAMB_NOTIFY_RET_TYPE(lvn_od_find_item, LVN_ODFINDITEMW, int)

	/// Adds a handler to LVN_ODSTATECHANGED wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_od_state_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_od_state_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-odstatechanged
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_od_state_changed, LVN_ODSTATECHANGED)

	/// Adds a handler to LVN_SETDISPINFO wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::lvn_set_disp_info)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::lvn_set_disp_info p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/lvn-setdispinfo
	WINLAMB_NOTIFY_RET_DEFAULT(lvn_set_disp_info, LVN_SETDISPINFOW)

	/// Adds a handler to NM_CLICK wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::nm_click)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::nm_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-click-list-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_click, NM_CLICK)

	/// Adds a handler to NM_CUSTOMDRAW wl::list_view notification.
	/// @tparam F `std::function<DWORD(msg::list_view::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::list_view::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-list-view
	WINLAMB_NOTIFY_RET_TYPE(nm_custom_draw, NM_CUSTOMDRAW, DWORD)

	/// Adds a handler to NM_DBLCLK wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::nm_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::nm_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-dblclk-list-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_dbl_clk, NM_DBLCLK)

	/// Adds a handler to NM_HOVER wl::list_view notification.
	/// @tparam F `std::function<int(msg::list_view::nm_hover)>` or `std::function<int()>`
	/// @param func: `[](msg::list_view::nm_hover p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-hover-list-view
	WINLAMB_NOTIFY_RET_TYPE(nm_hover, NM_HOVER, int)

	/// Adds a handler to NM_KILLFOCUS wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::nm_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::nm_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-killfocus-list-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_kill_focus, NM_KILLFOCUS)

	/// Adds a handler to NM_RCLICK wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::nm_r_click)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::nm_r_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rclick-list-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_r_click, NM_RCLICK)

	/// Adds a handler to NM_RDBLCLK wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::nm_r_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::nm_r_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rdblclk-list-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_r_dbl_clk, NM_RDBLCLK)

	/// Adds a handler to NM_RELEASEDCAPTURE wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::nm_released_capture)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::nm_released_capture p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-releasedcapture-list-view-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_released_capture, NM_RELEASEDCAPTURE)

	/// Adds a handler to NM_RETURN wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::nm_return)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::nm_return p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-return-list-view-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_return, NM_RETURN)

	/// Adds a handler to NM_SETFOCUS wl::list_view notification.
	/// @tparam F `std::function<void(msg::list_view::nm_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::list_view::nm_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-setfocus-list-view-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_set_focus, NM_SETFOCUS)
};

}//namespace wl::msg::list_view

namespace wl::msg::month_calendar {

/// Exposes handler methods to wl::month_calendar notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to MCN_GETDAYSTATE wl::month_calendar notification.
	/// @tparam F `std::function<void(msg::month_calendar::mcn_get_day_state)>` or `std::function<void()>`
	/// @param func: `[](msg::month_calendar::mcn_get_day_state p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/mcn-getdaystate
	WINLAMB_NOTIFY_RET_DEFAULT(mcn_get_day_state, MCN_GETDAYSTATE)

	/// Adds a handler to MCN_SELCHANGE wl::month_calendar notification.
	/// @tparam F `std::function<void(msg::month_calendar::mcn_sel_change)>` or `std::function<void()>`
	/// @param func: `[](msg::month_calendar::mcn_sel_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/mcn-selchange
	WINLAMB_NOTIFY_RET_DEFAULT(mcn_sel_change, MCN_SELCHANGE)

	/// Adds a handler to MCN_SELECT wl::month_calendar notification.
	/// @tparam F `std::function<void(msg::month_calendar::mcn_select)>` or `std::function<void()>`
	/// @param func: `[](msg::month_calendar::mcn_select p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/mcn-select
	WINLAMB_NOTIFY_RET_DEFAULT(mcn_select, MCN_SELECT)

	/// Adds a handler to MCN_VIEWCHANGE wl::month_calendar notification.
	/// @tparam F `std::function<void(msg::month_calendar::mcn_view_change)>` or `std::function<void()>`
	/// @param func: `[](msg::month_calendar::mcn_view_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/mcn-viewchange
	WINLAMB_NOTIFY_RET_DEFAULT(mcn_view_change, MCN_VIEWCHANGE)

	/// Adds a handler to NM_RELEASEDCAPTURE wl::month_calendar notification.
	/// @tparam F `std::function<void(msg::month_calendar::nm_released_capture)>` or `std::function<void()>`
	/// @param func: `[](msg::month_calendar::nm_released_capture p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-releasedcapture-monthcal-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_released_capture, NM_RELEASEDCAPTURE)
};

}//namespace wl::msg::month_calendar

namespace wl::msg::rebar {

/// Exposes handler methods to wl::rebar notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to RBN_AUTOBREAK wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_auto_break)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_auto_break p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-autobreak
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_auto_break, RBN_AUTOBREAK)

	/// Adds a handler to RBN_AUTOSIZE wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_auto_size)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_auto_size p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-autosize
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_auto_size, RBN_AUTOSIZE)

	/// Adds a handler to RBN_BEGINDRAG wl::rebar notification.
	/// @tparam F `std::function<int(msg::rebar::rbn_begin_drag)>` or `std::function<int()>`
	/// @param func: `[](msg::rebar::rbn_begin_drag p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-begindrag
	WINLAMB_NOTIFY_RET_TYPE(rbn_begin_drag, RBN_BEGINDRAG, int)

	/// Adds a handler to RBN_CHEVRONPUSHED wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_chevron_pushed)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_chevron_pushed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-chevronpushed
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_chevron_pushed, RBN_CHEVRONPUSHED)

	/// Adds a handler to RBN_CHILDSIZE wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_child_size)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_child_size p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-childsize
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_child_size, RBN_CHILDSIZE)

	/// Adds a handler to RBN_DELETEDBAND wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_deleted_band)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_deleted_band p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-deletedband
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_deleted_band, RBN_DELETEDBAND)

	/// Adds a handler to RBN_DELETINGDBAND wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_deleting_band)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_deleting_band p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-deletingband
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_deleting_band, RBN_DELETINGBAND)

	/// Adds a handler to RBN_ENDDRAG wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_end_drag)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_end_drag p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-enddrag
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_end_drag, RBN_ENDDRAG)

	/// Adds a handler to RBN_GETOBJECT wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_get_object)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_get_object p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-getobject
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_get_object, RBN_GETOBJECT)

	/// Adds a handler to RBN_HEIGHTCHANGE wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_height_change)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_height_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-heightchange
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_height_change, RBN_HEIGHTCHANGE)

	/// Adds a handler to RBN_LAYOUTCHANGED wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_layout_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_layout_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-layoutchanged
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_layout_changed, RBN_LAYOUTCHANGED)

	/// Adds a handler to RBN_MINMAX wl::rebar notification.
	/// @tparam F `std::function<int(msg::rebar::rbn_min_max)>` or `std::function<int()>`
	/// @param func: `[](msg::rebar::rbn_min_max p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-minmax
	WINLAMB_NOTIFY_RET_TYPE(rbn_min_max, RBN_MINMAX, int)

	/// Adds a handler to RBN_SPLITTERDRAG wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::rbn_splitter_drag)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::rbn_splitter_drag p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/rbn-splitterdrag
	WINLAMB_NOTIFY_RET_DEFAULT(rbn_splitter_drag, RBN_SPLITTERDRAG)

	/// Adds a handler to NM_CUSTOMDRAW wl::rebar notification.
	/// @tparam F `std::function<DWORD(msg::rebar::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::rebar::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-rebar
	WINLAMB_NOTIFY_RET_TYPE(nm_custom_draw, NM_CUSTOMDRAW, DWORD)

	/// Adds a handler to NM_NCHITTEST wl::rebar notification.
	/// @tparam F `std::function<int(msg::rebar::nm_nc_hit_test)>` or `std::function<int()>`
	/// @param func: `[](msg::rebar::nm_nc_hit_test p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-nchittest-rebar
	WINLAMB_NOTIFY_RET_TYPE(nm_nc_hit_test, NM_NCHITTEST, int)

	/// Adds a handler to NM_RELEASEDCAPTURE wl::rebar notification.
	/// @tparam F `std::function<void(msg::rebar::nm_released_capture)>` or `std::function<void()>`
	/// @param func: `[](msg::rebar::nm_released_capture p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-releasedcapture-rebar-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_released_capture, NM_RELEASEDCAPTURE)
};

}//namespace wl::msg::rebar

namespace wl::msg::status_bar {

/// Exposes handler methods to wl::status_bar notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to SBN_SIMPLEMODECHANGE wl::status_bar notification.
	/// @tparam F `std::function<void(msg::status_bar::sbn_simple_mode_change)>` or `std::function<void()>`
	/// @param func: `[](msg::status_bar::sbn_simple_mode_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/sbn-simplemodechange
	WINLAMB_NOTIFY_RET_DEFAULT(sbn_simple_mode_change, SBN_SIMPLEMODECHANGE)

	/// Adds a handler to NM_CLICK wl::status_bar notification.
	/// @tparam F `std::function<void(msg::status_bar::nm_click)>` or `std::function<void()>`
	/// @param func: `[](msg::status_bar::nm_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-click-status-bar
	WINLAMB_NOTIFY_RET_DEFAULT(nm_click, NM_CLICK)

	/// Adds a handler to NM_DBLCLK wl::status_bar notification.
	/// @tparam F `std::function<void(msg::status_bar::nm_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::status_bar::nm_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-dblclk-status-bar
	WINLAMB_NOTIFY_RET_DEFAULT(nm_dbl_clk, NM_DBLCLK)

	/// Adds a handler to NM_RCLICK wl::status_bar notification.
	/// @tparam F `std::function<void(msg::status_bar::nm_r_click)>` or `std::function<void()>`
	/// @param func: `[](msg::status_bar::nm_r_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rclick-status-bar
	WINLAMB_NOTIFY_RET_DEFAULT(nm_r_click, NM_RCLICK)

	/// Adds a handler to NM_RDBLCLK wl::status_bar notification.
	/// @tparam F `std::function<void(msg::status_bar::nm_r_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::status_bar::nm_r_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rdblclk-status-bar
	WINLAMB_NOTIFY_RET_DEFAULT(nm_r_dbl_clk, NM_RDBLCLK)
};

}//namespace wl::msg::status_bar

namespace wl::msg::sys_link {

/// Exposes handler methods to wl::sys_link notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to NM_CLICK wl::sys_link notification.
	/// @tparam F `std::function<void(msg::sys_link::nm_click)>` or `std::function<void()>`
	/// @param func: `[](msg::sys_link::nm_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-click-syslink
	WINLAMB_NOTIFY_RET_DEFAULT(nm_click, NM_CLICK)
};

}//namespace wl::msg::sys_link

namespace wl::msg::tab {

/// Exposes handler methods to wl::tab notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to TCN_FOCUSCHANGE wl::tab notification.
	/// @tparam F `std::function<void(msg::tab::tcn_focus_change)>` or `std::function<void()>`
	/// @param func: `[](msg::tab::tcn_focus_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tcn-focuschange
	WINLAMB_NOTIFY_RET_DEFAULT(tcn_focus_change, TCN_FOCUSCHANGE)

	/// Adds a handler to TCN_GETOBJECT wl::tab notification.
	/// @tparam F `std::function<void(msg::tab::tcn_get_object)>` or `std::function<void()>`
	/// @param func: `[](msg::tab::tcn_get_object p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tcn-getobject
	WINLAMB_NOTIFY_RET_DEFAULT(tcn_get_object, TCN_GETOBJECT)

	/// Adds a handler to TCN_KEYDOWN wl::tab notification.
	/// @tparam F `std::function<void(msg::tab::tcn_key_down)>` or `std::function<void()>`
	/// @param func: `[](msg::tab::tcn_key_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tcn-keydown
	WINLAMB_NOTIFY_RET_DEFAULT(tcn_key_down, TCN_KEYDOWN)

	/// Adds a handler to TCN_SELCHANGE wl::tab notification.
	/// @tparam F `std::function<void(msg::tab::tcn_sel_change)>` or `std::function<void()>`
	/// @param func: `[](msg::tab::tcn_sel_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tcn-selchange
	WINLAMB_NOTIFY_RET_DEFAULT(tcn_sel_change, TCN_SELCHANGE)

	/// Adds a handler to TCN_SELCHANGING wl::tab notification.
	/// @tparam F `std::function<bool(msg::tab::tcn_sel_changing)>` or `std::function<bool()>`
	/// @param func: `[](msg::tab::tcn_sel_changing p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tcn-selchanging
	WINLAMB_NOTIFY_RET_TYPE(tcn_sel_changing, TCN_SELCHANGING, bool)

	/// Adds a handler to NM_CLICK wl::tab notification.
	/// @tparam F `std::function<void(msg::tab::nm_click)>` or `std::function<void()>`
	/// @param func: `[](msg::tab::nm_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-click-tab
	WINLAMB_NOTIFY_RET_DEFAULT(nm_click, NM_CLICK)

	/// Adds a handler to NM_DBLCLK wl::tab notification.
	/// @tparam F `std::function<int(msg::tab::nm_dbl_clk)>` or `std::function<int()>`
	/// @param func: `[](msg::tab::nm_dbl_clk p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-dblclk-tab
	WINLAMB_NOTIFY_RET_TYPE(nm_dbl_clk, NM_DBLCLK, int)

	/// Adds a handler to NM_RCLICK wl::tab notification.
	/// @tparam F `std::function<int(msg::tab::nm_r_click)>` or `std::function<int()>`
	/// @param func: `[](msg::tab::nm_r_click p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rclick-tab
	WINLAMB_NOTIFY_RET_TYPE(nm_r_click, NM_RCLICK, int)

	/// Adds a handler to NM_RDBLCLK wl::tab notification.
	/// @tparam F `std::function<int(msg::tab::nm_r_dbl_clk)>` or `std::function<int()>`
	/// @param func: `[](msg::tab::nm_r_dbl_clk p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rdblclk-tab
	WINLAMB_NOTIFY_RET_TYPE(nm_r_dbl_clk, NM_RDBLCLK, int)

	/// Adds a handler to NM_RELEASEDCAPTURE wl::tab notification.
	/// @tparam F `std::function<void(msg::tab::nm_released_capture)>` or `std::function<void()>`
	/// @param func: `[](msg::tab::nm_released_capture p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-releasedcapture-tab-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_released_capture, NM_RELEASEDCAPTURE)
};

}//namespace wl::msg::tab

namespace wl::msg::tool_bar {

/// Exposes handler methods to wl::tool_bar notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to TBN_BEGINADJUST wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_begin_adjust)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_begin_adjust p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-beginadjust
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_begin_adjust, TBN_BEGINADJUST)

	/// Adds a handler to TBN_BEGINDRAG wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_begin_drag)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_begin_drag p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-begindrag
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_begin_drag, TBN_BEGINDRAG)

	/// Adds a handler to TBN_CUSTHELP wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_cust_help)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_cust_help p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-custhelp
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_cust_help, TBN_CUSTHELP)

	/// Adds a handler to TBN_DELETINGBUTTON wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_deleting_button)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_deleting_button p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-deletingbutton
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_deleting_button, TBN_DELETINGBUTTON)

	/// Adds a handler to TBN_DRAGOUT wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_drag_out)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_drag_out p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-dragout
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_drag_out, TBN_DRAGOUT)

	/// Adds a handler to TBN_DRAGOVER wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::tbn_drag_over)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::tbn_drag_over p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-dragover
	WINLAMB_NOTIFY_RET_TYPE(tbn_drag_over, TBN_DRAGOVER, bool)

	/// Adds a handler to TBN_DROPDOWN wl::tool_bar notification.
	/// @tparam F `std::function<BYTE(msg::tool_bar::tbn_drop_down)>` or `std::function<BYTE()>`
	/// @param func: `[](msg::tool_bar::tbn_drop_down p) -> BYTE {}` or `[]() -> BYTE {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-dropdown
	WINLAMB_NOTIFY_RET_TYPE(tbn_drop_down, TBN_DROPDOWN, BYTE)

	/// Adds a handler to TBN_DUPACCELERATOR wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::tbn_dup_accelerator)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::tbn_dup_accelerator p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-dupaccelerator
	WINLAMB_NOTIFY_RET_TYPE(tbn_dup_accelerator, TBN_DUPACCELERATOR, bool)

	/// Adds a handler to TBN_ENDADJUST wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_end_adjust)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_end_adjust p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-endadjust
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_end_adjust, TBN_ENDADJUST)

	/// Adds a handler to TBN_ENDDRAG wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_end_drag)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_end_drag p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-enddrag
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_end_drag, TBN_ENDDRAG)

	/// Adds a handler to TBN_GETBUTTONINFO wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::tbn_get_button_info)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::tbn_get_button_info p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-getbuttoninfo
	WINLAMB_NOTIFY_RET_TYPE(tbn_get_button_info, TBN_GETBUTTONINFO, bool)

	/// Adds a handler to TBN_GETDISPINFO wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_get_disp_info)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_get_disp_info p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-getdispinfo
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_get_disp_info, TBN_GETDISPINFO)

	/// Adds a handler to TBN_GETINFOTIP wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_get_info_tip)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_get_info_tip p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-getinfotip
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_get_info_tip, TBN_GETINFOTIP)

	/// Adds a handler to TBN_GETOBJECT wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_get_object)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_get_object p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-getobject
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_get_object, TBN_GETOBJECT)

	/// Adds a handler to TBN_HOTITEMCHANGE rebar notification.
	/// @tparam F `std::function<int(msg::tool_bar::tbn_hot_item_change)>` or `std::function<int()>`
	/// @param func: `[](msg::tool_bar::tbn_hot_item_change p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-hotitemchange
	WINLAMB_NOTIFY_RET_TYPE(tbn_hot_item_change, TBN_HOTITEMCHANGE, int)

	/// Adds a handler to TBN_INITCUSTOMIZE wl::tool_bar notification.
	/// @tparam F `std::function<BYTE(msg::tool_bar::tbn_init_customize)>` or `std::function<BYTE()>`
	/// @param func: `[](msg::tool_bar::tbn_init_customize p) -> BYTE {}` or `[]() -> BYTE {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-initcustomize
	WINLAMB_NOTIFY_RET_TYPE(tbn_init_customize, TBN_INITCUSTOMIZE, BYTE)

	/// Adds a handler to TBN_MAPACCELERATOR wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::tbn_map_accelerator)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::tbn_map_accelerator p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-mapaccelerator
	WINLAMB_NOTIFY_RET_TYPE(tbn_map_accelerator, TBN_MAPACCELERATOR, bool)

	/// Adds a handler to TBN_QUERYDELETE wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::tbn_query_delete)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::tbn_query_delete p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-querydelete
	WINLAMB_NOTIFY_RET_TYPE(tbn_query_delete, TBN_QUERYDELETE, bool)

	/// Adds a handler to TBN_QUERYINSERT wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::tbn_query_insert)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::tbn_query_insert p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-queryinsert
	WINLAMB_NOTIFY_RET_TYPE(tbn_query_insert, TBN_QUERYINSERT, bool)

	/// Adds a handler to TBN_RESET wl::tool_bar notification.
	/// @tparam F `std::function<BYTE(msg::tool_bar::tbn_reset)>` or `std::function<BYTE()>`
	/// @param func: `[](msg::tool_bar::tbn_reset p) -> BYTE {}` or `[]() -> BYTE {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-reset
	WINLAMB_NOTIFY_RET_TYPE(tbn_reset, TBN_RESET, BYTE)

	/// Adds a handler to TBN_RESTORE rebar notification.
	/// @tparam F `std::function<int(msg::tool_bar::tbn_restore)>` or `std::function<int()>`
	/// @param func: `[](msg::tool_bar::tbn_restore p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-restore
	WINLAMB_NOTIFY_RET_TYPE(tbn_restore, TBN_RESTORE, int)

	/// Adds a handler to TBN_SAVE wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_save)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_save p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-save
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_save, TBN_SAVE)

	/// Adds a handler to TBN_tool_barCHANGE wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::tbn_tool_bar_change)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::tbn_tool_bar_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-tool_barchange
	WINLAMB_NOTIFY_RET_DEFAULT(tbn_toolbar_change, TBN_TOOLBARCHANGE)

	/// Adds a handler to TBN_WRAPACCELERATOR wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::tbn_wrap_accelerator)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::tbn_wrap_accelerator p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-wrapaccelerator
	WINLAMB_NOTIFY_RET_TYPE(tbn_wrap_accelerator, TBN_WRAPACCELERATOR, bool)

	/// Adds a handler to TBN_WRAPHOTITEM wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::tbn_wrap_hot_item)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::tbn_wrap_hot_item p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tbn-wraphotitem
	WINLAMB_NOTIFY_RET_TYPE(tbn_wrap_hot_item, TBN_WRAPHOTITEM, bool)

	/// Adds a handler to NM_CHAR wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::nm_char)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::nm_char p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-char-tool_bar
	WINLAMB_NOTIFY_RET_TYPE(nm_char, NM_CHAR, bool)

	/// Adds a handler to NM_CLICK wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::nm_click)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::nm_click p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-click-tool_bar
	WINLAMB_NOTIFY_RET_TYPE(nm_click, NM_CLICK, bool)

	/// Adds a handler to NM_CUSTOMDRAW wl::tool_bar notification.
	/// @tparam F `std::function<DWORD(msg::tool_bar::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::tool_bar::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-tool_bar
	WINLAMB_NOTIFY_RET_TYPE(nm_custom_draw, NM_CUSTOMDRAW, DWORD)

	/// Adds a handler to NM_DBLCLK wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::nm_dbl_clk)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::nm_dbl_clk p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-dblclk-tool_bar
	WINLAMB_NOTIFY_RET_TYPE(nm_dbl_clk, NM_DBLCLK, bool)

	/// Adds a handler to NM_KEYDOWN wl::tool_bar notification.
	/// @tparam F `std::function<int(msg::tool_bar::nm_key_down)>` or `std::function<int()>`
	/// @param func: `[](msg::tool_bar::nm_key_down p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-keydown-tool_bar
	WINLAMB_NOTIFY_RET_TYPE(nm_key_down, NM_KEYDOWN, int)

	/// Adds a handler to NM_LDOWN wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::nm_l_down)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::nm_l_down p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-ldown-tool_bar
	WINLAMB_NOTIFY_RET_TYPE(nm_l_down, NM_LDOWN, bool)

	/// Adds a handler to NM_RCLICK wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::nm_r_click)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::nm_r_click p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rclick-tool_bar
	WINLAMB_NOTIFY_RET_TYPE(nm_r_click, NM_RCLICK, bool)

	/// Adds a handler to NM_RDBLCLK wl::tool_bar notification.
	/// @tparam F `std::function<bool(msg::tool_bar::nm_r_dbl_clk)>` or `std::function<bool()>`
	/// @param func: `[](msg::tool_bar::nm_r_dbl_clk p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rdblclk-tool_bar
	WINLAMB_NOTIFY_RET_TYPE(nm_r_dbl_clk, NM_RDBLCLK, bool)

	/// Adds a handler to NM_RELEASEDCAPTURE wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::nm_released_capture)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::nm_released_capture p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-releasedcapture-tool_bar-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_released_capture, NM_RELEASEDCAPTURE)

	/// Adds a handler to NM_TOOLTIPSCREATED wl::tool_bar notification.
	/// @tparam F `std::function<void(msg::tool_bar::nm_tooltips_created)>` or `std::function<void()>`
	/// @param func: `[](msg::tool_bar::nm_tooltips_created p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-tooltipscreated-tool_bar-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_tooltips_created, NM_TOOLTIPSCREATED)
};

}//namespace wl::msg::tool_bar

namespace wl::msg::tooltip {

/// Exposes handler methods to wl::tooltip notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to TTN_GETDISPINFO wl::tooltip notification.
	/// @tparam F `std::function<void(msg::tooltip::ttn_get_disp_info)>` or `std::function<void()>`
	/// @param func: `[](msg::tooltip::ttn_get_disp_info p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ttn-getdispinfo
	WINLAMB_NOTIFY_RET_DEFAULT(ttn_get_disp_info, TTN_GETDISPINFOW)

	/// Adds a handler to TTN_LINKCLICK wl::tooltip notification.
	/// @tparam F `std::function<void(msg::tooltip::ttn_link_click)>` or `std::function<void()>`
	/// @param func: `[](msg::tooltip::ttn_link_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ttn-linkclick
	WINLAMB_NOTIFY_RET_DEFAULT(ttn_link_click, TTN_LINKCLICK)

	/// Adds a handler to TTN_NEEDTEXT wl::tooltip notification.
	/// @tparam F `std::function<void(msg::tooltip::ttn_need_text)>` or `std::function<void()>`
	/// @param func: `[](msg::tooltip::ttn_need_text p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ttn-needtext
	WINLAMB_NOTIFY_RET_DEFAULT(ttn_need_text, TTN_NEEDTEXTW)

	/// Adds a handler to TTN_POP wl::tooltip notification.
	/// @tparam F `std::function<void(msg::tooltip::ttn_pop)>` or `std::function<void()>`
	/// @param func: `[](msg::tooltip::ttn_pop p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ttn-pop
	WINLAMB_NOTIFY_RET_DEFAULT(ttn_pop, TTN_POP)

	/// Adds a handler to TTN_SHOW wl::tooltip notification.
	/// @tparam F `std::function<void(msg::tooltip::ttn_show)>` or `std::function<void()>`
	/// @param func: `[](msg::tooltip::ttn_show p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/ttn-show
	WINLAMB_NOTIFY_RET_DEFAULT(ttn_show, TTN_SHOW)

	/// Adds a handler to NM_CUSTOMDRAW wl::tooltip notification.
	/// @tparam F `std::function<DWORD(msg::tooltip::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::tooltip::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-tooltip
	WINLAMB_NOTIFY_RET_TYPE(nm_custom_draw, NM_CUSTOMDRAW, DWORD)
};

}//namespace wl::msg::tooltip

namespace wl::msg::track_bar {

/// Exposes handler methods to wl::track_bar notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to TRBN_THUMBPOSCHANGING wl::track_bar notification.
	/// @tparam F `std::function<void(msg::track_bar::trbn_thumb_pos_changing)>` or `std::function<void()>`
	/// @param func: `[](msg::track_bar::trbn_thumb_pos_changing p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/trbn-thumbposchanging
	WINLAMB_NOTIFY_RET_DEFAULT(trbn_thumb_pos_changing, TRBN_THUMBPOSCHANGING)

	/// Adds a handler to NM_CUSTOMDRAW wl::track_bar notification.
	/// @tparam F `std::function<DWORD(msg::track_bar::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::track_bar::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-trackbar
	WINLAMB_NOTIFY_RET_TYPE(nm_custom_draw, NM_CUSTOMDRAW, DWORD)

	/// Adds a handler to NM_RELEASEDCAPTURE wl::track_bar notification.
	/// @tparam F `std::function<void(msg::track_bar::nm_released_capture)>` or `std::function<void()>`
	/// @param func: `[](msg::track_bar::nm_released_capture p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-releasedcapture-trackbar-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_released_capture, NM_RELEASEDCAPTURE)
};

}//namespace wl::msg::track_bar

namespace wl::msg::tree_view {

/// Exposes handler methods to wl::tree_view notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to TVN_ASYNCDRAW wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_async_draw)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_async_draw p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-asyncdraw
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_async_draw, TVN_ASYNCDRAW)

	/// Adds a handler to TVN_BEGINDRAG wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_begin_drag)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_begin_drag p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-begindrag
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_begin_drag, TVN_BEGINDRAGW)

	/// Adds a handler to TVN_BEGINLABELEDIT wl::tree_view notification.
	/// @tparam F `std::function<bool(msg::tree_view::tvn_begin_label_edit)>` or `std::function<bool()>`
	/// @param func: `[](msg::tree_view::tvn_begin_label_edit p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-beginlabeledit
	WINLAMB_NOTIFY_RET_TYPE(tvn_begin_label_edit, TVN_BEGINLABELEDITW, bool)

	/// Adds a handler to TVN_BEGINRDRAG wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_begin_r_drag)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_begin_r_drag p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-beginrdrag
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_begin_r_drag, TVN_BEGINRDRAGW)

	/// Adds a handler to TVN_DELETEITEM wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_delete_item)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_delete_item p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-deleteitem
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_delete_item, TVN_DELETEITEMW)

	/// Adds a handler to TVN_ENDLABELEDIT wl::tree_view notification.
	/// @tparam F `std::function<bool(msg::tree_view::tvn_end_label_edit)>` or `std::function<bool()>`
	/// @param func: `[](msg::tree_view::tvn_end_label_edit p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-endlabeledit
	WINLAMB_NOTIFY_RET_TYPE(tvn_end_label_edit, TVN_ENDLABELEDITW, bool)

	/// Adds a handler to TVN_GETDISPINFO wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_get_disp_info)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_get_disp_info p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-getdispinfo
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_get_disp_info, TVN_GETDISPINFOW)

	/// Adds a handler to TVN_GETINFOTIP wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_get_info_tip)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_get_info_tip p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-getinfotip
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_get_info_tip, TVN_GETINFOTIPW)

	/// Adds a handler to TVN_ITEMCHANGED wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_item_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_item_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-itemchanged
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_item_changed, TVN_ITEMCHANGEDW)

	/// Adds a handler to TVN_ITEMCHANGING wl::tree_view notification.
	/// @tparam F `std::function<bool(msg::tree_view::tvn_item_changing)>` or `std::function<bool()>`
	/// @param func: `[](msg::tree_view::tvn_item_changing p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-itemchanging
	WINLAMB_NOTIFY_RET_TYPE(tvn_item_changing, TVN_ITEMCHANGINGW, bool)

	/// Adds a handler to TVN_ITEMEXPANDED wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_item_expanded)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_item_expanded p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-itemexpanded
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_item_expanded, TVN_ITEMEXPANDEDW)

	/// Adds a handler to TVN_ITEMEXPANDING wl::tree_view notification.
	/// @tparam F `std::function<bool(msg::tree_view::tvn_item_expanding)>` or `std::function<bool()>`
	/// @param func: `[](msg::tree_view::tvn_item_expanding p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-itemexpanding
	WINLAMB_NOTIFY_RET_TYPE(tvn_item_expanding, TVN_ITEMEXPANDINGW, bool)

	/// Adds a handler to TVN_KEYDOWN wl::tree_view notification.
	/// @tparam F `std::function<bool(msg::tree_view::tvn_key_down)>` or `std::function<bool()>`
	/// @param func: `[](msg::tree_view::tvn_key_down p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-keydown
	WINLAMB_NOTIFY_RET_TYPE(tvn_key_down, TVN_KEYDOWN, bool)

	/// Adds a handler to TVN_SELCHANGED wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_sel_changed)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_sel_changed p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-selchanged
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_sel_changed, TVN_SELCHANGEDW)

	/// Adds a handler to TVN_SELCHANGING wl::tree_view notification.
	/// @tparam F `std::function<bool(msg::tree_view::tvn_sel_changing)>` or `std::function<bool()>`
	/// @param func: `[](msg::tree_view::tvn_sel_changing p) -> bool {}` or `[]() -> bool {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-selchanging
	WINLAMB_NOTIFY_RET_TYPE(tvn_sel_changing, TVN_SELCHANGINGW, bool)

	/// Adds a handler to TVN_SETDISPINFO wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::tvn_set_disp_info)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::tvn_set_disp_info p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-setdispinfo
	WINLAMB_NOTIFY_RET_DEFAULT(tvn_set_disp_info, TVN_SETDISPINFOW)

	/// Adds a handler to TVN_SINGLEEXPAND wl::tree_view notification.
	/// @tparam F `std::function<BYTE(msg::tree_view::tvn_single_expand)>` or `std::function<BYTE()>`
	/// @param func: `[](msg::tree_view::tvn_single_expand p) -> BYTE {}` or `[]() -> BYTE {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/tvn-singleexpand
	WINLAMB_NOTIFY_RET_TYPE(tvn_single_expand, TVN_SINGLEEXPAND, BYTE)

	/// Adds a handler to NM_CLICK wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::nm_click)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::nm_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-click-tree-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_click, NM_CLICK)

	/// Adds a handler to NM_CUSTOMDRAW wl::tree_view notification.
	/// @tparam F `std::function<DWORD(msg::tree_view::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::tree_view::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-tree-view
	WINLAMB_NOTIFY_RET_TYPE(nm_custom_draw, NM_CUSTOMDRAW, DWORD)

	/// Adds a handler to NM_DBLCLK wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::nm_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::nm_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-dblclk-tree-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_dbl_clk, NM_DBLCLK)

	/// Adds a handler to NM_KILLFOCUS wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::nm_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::nm_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-killfocus-tree-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_kill_focus, NM_KILLFOCUS)

	/// Adds a handler to NM_RCLICK wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::nm_r_click)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::nm_r_click p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rclick-tree-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_r_click, NM_RCLICK)

	/// Adds a handler to NM_RDBLCLK wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::nm_r_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::nm_r_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-rdblclk-tree-view
	WINLAMB_NOTIFY_RET_DEFAULT(nm_r_dbl_clk, NM_RDBLCLK)

	/// Adds a handler to NM_RETURN wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::nm_return)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::nm_return p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-return-tree-view-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_return, NM_RETURN)

	/// Adds a handler to NM_SETCURSOR wl::tree_view notification.
	/// @tparam F `std::function<int(msg::tree_view::nm_set_cursor)>` or `std::function<int()>`
	/// @param func: `[](msg::tree_view::nm_set_cursor p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-setcursor-tree-view-
	WINLAMB_NOTIFY_RET_TYPE(nm_set_cursor, NM_SETCURSOR, int)

	/// Adds a handler to NM_SETFOCUS wl::tree_view notification.
	/// @tparam F `std::function<void(msg::tree_view::nm_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::tree_view::nm_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-setfocus-tree-view-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_set_focus, NM_SETFOCUS)
};

}//namespace wl::msg::tree_view

namespace wl::msg::up_down {

/// Exposes handler methods to wl::up_down notifications.
class wnd_events final {
private:
	_wli::base_native_control& _base;

public:
	wnd_events(_wli::base_native_control& base) noexcept
		: _base{base} { }

	/// Adds a handler to UDN_DELTAPOS wl::up_down notification.
	/// @tparam F `std::function<int(msg::up_down::udn_delta_pos)>` or `std::function<int()>`
	/// @param func: `[](msg::up_down::udn_delta_pos p) -> int {}` or `[]() -> int {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/udn-deltapos
	WINLAMB_NOTIFY_RET_TYPE(udn_delta_pos, UDN_DELTAPOS, int)

	/// Adds a handler to NM_RELEASEDCAPTURE wl::up_down notification.
	/// @tparam F `std::function<void(msg::up_down::nm_released_capture)>` or `std::function<void()>`
	/// @param func: `[](msg::up_down::nm_released_capture p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-releasedcapture-up-down-
	WINLAMB_NOTIFY_RET_DEFAULT(nm_released_capture, NM_RELEASEDCAPTURE)
};

}//namespace wl::msg::up_down

#undef WINLAMB_NOTIFY_RET_TYPE
#undef WINLAMB_NOTIFY_RET_DEFAULT
#undef WINLAMB_COMMAND_RET_DEFAULT

// The for loops below don't use braces because of a bug in Doxygen.
// https://sourceforge.net/p/doxygen/mailman/message/12235945/

#define WINLAMB_RADIO_GROUP_RET_DEFAULT(methodName) \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, void(methodName), void) \
	{ \
		for (wnd_events* itemEvent : this->_itemEvents) \
			itemEvent->methodName(std::forward<F>(func)); \
	} \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, void(), void) \
	{ \
		for (wnd_events* itemEvent : this->_itemEvents) \
			itemEvent->methodName(std::forward<F>(func)); \
	}

#define WINLAMB_RADIO_GROUP_RET_TYPE(methodName, retType) \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, retType(methodName), void) \
	{ \
		for (wnd_events* itemEvent : this->_itemEvents) \
			itemEvent->methodName(std::forward<F>(func)); \
	} \
	template<typename F> \
	auto methodName(F&& func) \
		-> WINLAMB_LAMBDA_TYPE(func, retType(), void) \
	{ \
		for (wnd_events* itemEvent : this->_itemEvents) \
			itemEvent->methodName(std::forward<F>(func)); \
	}

namespace wl { class radio_group; } // forward declaration

namespace wl::msg::button {

/// Exposes handler methods to wl::button notifications,
/// for each wl::radio_button in the wl::radio_group.
class wnd_events_rg final {
private:
	friend wl::radio_group;
	std::vector<button::wnd_events*> _itemEvents; // filled by friend radio_group

public:
	/// Adds a handler to BCN_DROPDOWN wl::button notification, to each wl::radio_button in the group.
	/// @tparam F `std::function<void(msg::button::bcn_drop_down)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bcn_drop_down p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bcn-dropdown
	WINLAMB_RADIO_GROUP_RET_DEFAULT(bcn_drop_down)

	/// Adds a handler to BCN_HOTITEMCHANGE wl::button notification, to each wl::radio_button in the group.
	/// @tparam F `std::function<void(msg::button::bcn_hot_item_change)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bcn_hot_item_change p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bcn-hotitemchange
	WINLAMB_RADIO_GROUP_RET_DEFAULT(bcn_hot_item_change)

	/// Adds a handler to BN_CLICKED wl::button notification, to each wl::radio_button in the group.
	/// @tparam F `std::function<void(msg::button::bn_clicked)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bn_clicked p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bn-clicked
	WINLAMB_RADIO_GROUP_RET_DEFAULT(bn_clicked)

	/// Adds a handler to BN_DBLCLK wl::button notification, to each wl::radio_button in the group.
	/// @tparam F `std::function<void(msg::button::bn_dbl_clk)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bn_dbl_clk p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bn-dblclk
	WINLAMB_RADIO_GROUP_RET_DEFAULT(bn_dbl_clk)

	/// Adds a handler to BN_KILLFOCUS wl::button notification, to each wl::radio_button in the group.
	/// @tparam F `std::function<void(msg::button::bn_kill_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bn_kill_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bn-killfocus
	WINLAMB_RADIO_GROUP_RET_DEFAULT(bn_kill_focus)

	/// Adds a handler to BN_SETFOCUS wl::button notification, to each wl::radio_button in the group.
	/// @tparam F `std::function<void(msg::button::bn_set_focus)>` or `std::function<void()>`
	/// @param func: `[](msg::button::bn_set_focus p) {}` or `[]() {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/bn-setfocus
	WINLAMB_RADIO_GROUP_RET_DEFAULT(bn_set_focus)

	/// Adds a handler to NM_CUSTOMDRAW wl::button notification, to each wl::radio_button in the group.
	/// @tparam F `std::function<DWORD(msg::button::nm_custom_draw)>` or `std::function<DWORD()>`
	/// @param func: `[](msg::button::nm_custom_draw p) -> DWORD {}` or `[]() -> DWORD {}`
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/nm-customdraw-button
	WINLAMB_RADIO_GROUP_RET_TYPE(nm_custom_draw, DWORD)
};

}//namespace wl::msg::button

#undef WINLAMB_RADIO_GROUP_RET_TYPE
#undef WINLAMB_RADIO_GROUP_RET_DEFAULT