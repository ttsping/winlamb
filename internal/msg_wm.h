/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>
#include "../menu.h"

/// Structs that extract values from window messages.
namespace wl::msg {

/// Message parameters of any window message, raw WPARAM and LPARAM values.
struct wm {
	/// Raw, unprocessed WPARAM data.
	WPARAM wparam;
	/// Raw, unprocessed LPARAM data.
	LPARAM lparam;
};

#define WINLAMB_PARM_EMPTY(msgname) \
	struct msgname : public wm { \
		msgname(wm p) noexcept : wm{p} { } \
	};

#define WINLAMB_PARM_INHERIT(msgname, msgbase) \
	struct msgname : public msgbase { \
		msgname(wm p) noexcept : msgbase{p} { } \
	};

struct wm_activate : public wm {
	wm_activate(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_being_activated() const noexcept              { return LOWORD(this->wparam) != WA_INACTIVE; }
	[[nodiscard]] bool is_activated_not_by_mouse_click() const noexcept { return LOWORD(this->wparam) == WA_ACTIVE; }
	[[nodiscard]] bool is_activated_by_mouse_click() const noexcept     { return LOWORD(this->wparam) == WA_CLICKACTIVE; }
	[[nodiscard]] bool is_minimized() const noexcept                    { return HIWORD(this->wparam) != 0; }
	[[nodiscard]] HWND swapped_window() const noexcept                  { return reinterpret_cast<HWND>(this->lparam); }
};

struct wm_activate_app : public wm {
	wm_activate_app(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool  is_being_activated() const noexcept { return this->wparam != FALSE; }
	[[nodiscard]] DWORD thread_id() const noexcept          { return static_cast<DWORD>(this->lparam); }
};

struct wm_app_command : public wm {
	wm_app_command(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND owner_hwnd() const noexcept  { return reinterpret_cast<HWND>(this->wparam); }
	[[nodiscard]] WORD app_command() const noexcept { return GET_APPCOMMAND_LPARAM(this->lparam); }
	[[nodiscard]] WORD u_device() const noexcept    { return GET_DEVICE_LPARAM(this->lparam); }
	[[nodiscard]] WORD key_state() const noexcept   { return GET_KEYSTATE_LPARAM(this->lparam); }
};

struct wm_ask_cb_format_name : public wm {
	wm_ask_cb_format_name(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT     buffer_sz() const noexcept { return static_cast<UINT>(this->wparam); }
	[[nodiscard]] wchar_t* buffer() const noexcept    { return reinterpret_cast<wchar_t*>(this->lparam); }
};

WINLAMB_PARM_EMPTY(wm_cancel_mode)

struct wm_capture_changed : public wm {
	wm_capture_changed(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND hwnd_gaining_mouse() const noexcept { return reinterpret_cast<HWND>(this->lparam); }
};

struct wm_change_cb_chain : public wm {
	wm_change_cb_chain(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND hwnd_being_removed() const noexcept { return reinterpret_cast<HWND>(this->wparam); }
	[[nodiscard]] HWND next_hwnd() const noexcept          { return reinterpret_cast<HWND>(this->lparam); }
	[[nodiscard]] bool is_last_hwnd() const noexcept       { return this->next_hwnd() == nullptr; }
};

struct wm_char : public wm {
	wm_char(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD char_code() const noexcept               { return static_cast<WORD>(this->wparam); }
	[[nodiscard]] WORD repeat_count() const noexcept            { return LOWORD(this->lparam); }
	[[nodiscard]] BYTE scan_code() const noexcept               { return LOBYTE(HIWORD(this->lparam)); }
	[[nodiscard]] bool is_extended_key() const noexcept         { return (this->lparam >> 24) & 1; }
	[[nodiscard]] bool has_alt_key() const noexcept             { return (this->lparam >> 29) & 1; }
	[[nodiscard]] bool was_key_previously_down() const noexcept { return (this->lparam >> 30) & 1; }
	[[nodiscard]] bool is_key_being_released() const noexcept   { return (this->lparam >> 31) & 1; }
};

struct wm_char_to_item : public wm {
	wm_char_to_item(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD char_code() const noexcept         { return LOWORD(this->wparam); }
	[[nodiscard]] WORD current_caret_pos() const noexcept { return HIWORD(this->wparam); }
	[[nodiscard]] HWND hlistbox() const noexcept          { return reinterpret_cast<HWND>(this->lparam); }
};

WINLAMB_PARM_EMPTY(wm_child_activate)

WINLAMB_PARM_EMPTY(wm_clipboard_update)

WINLAMB_PARM_EMPTY(wm_close)

struct wm_command : public wm {
	wm_command(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_from_menu() const noexcept        { return !this->lparam && HIWORD(this->wparam) == 0; }
	[[nodiscard]] bool is_from_accelerator() const noexcept { return !this->lparam && HIWORD(this->wparam) == 1; }
	[[nodiscard]] bool is_from_control() const noexcept     { return !!this->lparam; }
	[[nodiscard]] WORD id() const noexcept                  { return LOWORD(this->wparam); }
	[[nodiscard]] int  control_notif_code() const noexcept  { return HIWORD(this->wparam); }
	[[nodiscard]] HWND control_hwnd() const noexcept        { return reinterpret_cast<HWND>(this->lparam); }
};

struct wm_command_accel : public wm {
	wm_command_accel(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD cmd_id() const noexcept { return LOWORD(this->wparam); }
};

struct wm_command_accel_menu : public wm {
	wm_command_accel_menu(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_from_menu() const noexcept        { return HIWORD(this->wparam) == 0; }
	[[nodiscard]] bool is_from_accelerator() const noexcept { return HIWORD(this->wparam) == 1; }
	[[nodiscard]] WORD cmd_id() const noexcept              { return LOWORD(this->wparam); }
};

WINLAMB_PARM_INHERIT(wm_command_menu, wm_command_accel)

struct wm_compacting : public wm {
	wm_compacting(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT cpu_time_ratio() const noexcept { return static_cast<UINT>(this->wparam); }
};

struct wm_compare_item : public wm {
	wm_compare_item(wm p) noexcept : wm{p} { }
	[[nodiscard]] int                      control_id() const noexcept        { return static_cast<int>(this->wparam); }
	[[nodiscard]] const COMPAREITEMSTRUCT& compareitemstruct() const noexcept { return *reinterpret_cast<const COMPAREITEMSTRUCT*>(this->lparam); }
};

struct wm_context_menu : public wm {
	wm_context_menu(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND  target() const noexcept { return reinterpret_cast<HWND>(this->wparam); }
	[[nodiscard]] POINT pos() const noexcept    { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

struct wm_copy_data : public wm {
	wm_copy_data(wm p) noexcept : wm{p} { }
	[[nodiscard]] const COPYDATASTRUCT& copydatastruct() const noexcept { return *reinterpret_cast<const COPYDATASTRUCT*>(this->lparam); }
};

struct wm_create : public wm {
	wm_create(wm p) noexcept : wm{p} { }
	[[nodiscard]] const CREATESTRUCTW& createstruct() const noexcept { return *reinterpret_cast<const CREATESTRUCTW*>(this->lparam); }
};

struct wm_ctl_color_btn : public wm {
	wm_ctl_color_btn(wm p) noexcept : wm{p} { }
	[[nodiscard]] HDC  hdc() const noexcept  { return reinterpret_cast<HDC>(this->wparam); }
	[[nodiscard]] HWND hctl() const noexcept { return reinterpret_cast<HWND>(this->lparam); }
};

WINLAMB_PARM_INHERIT(wm_ctl_color_dlg, wm_ctl_color_btn)

WINLAMB_PARM_INHERIT(wm_ctl_color_edit, wm_ctl_color_btn)

WINLAMB_PARM_INHERIT(wm_ctl_color_list_box, wm_ctl_color_btn)

WINLAMB_PARM_INHERIT(wm_ctl_color_scroll_bar, wm_ctl_color_btn)

WINLAMB_PARM_INHERIT(wm_ctl_color_static, wm_ctl_color_btn)

WINLAMB_PARM_INHERIT(wm_dead_char, wm_char)

struct wm_delete_item : public wm {
	wm_delete_item(wm p) noexcept : wm{p} { }
	[[nodiscard]] int                     control_id() const noexcept       { return static_cast<int>(this->wparam); }
	[[nodiscard]] const DELETEITEMSTRUCT& deleteitemstruct() const noexcept { return *reinterpret_cast<const DELETEITEMSTRUCT*>(this->lparam); }
};

WINLAMB_PARM_EMPTY(wm_destroy)

WINLAMB_PARM_EMPTY(wm_destroy_clipboard)

struct wm_dev_mode_change : public wm {
	wm_dev_mode_change(wm p) noexcept : wm{p} { }
	[[nodiscard]] const wchar_t* device_name() const noexcept { return reinterpret_cast<const wchar_t*>(this->lparam); }
};

#ifdef _DBT_H // Ras.h
struct wm_device_change : public wm {
	wm_device_change(wm p) : wm(p) { }
	[[nodiscard]] DWORD                                device_event() const noexcept                  { return static_cast<UINT>(this->wparam); }
	[[nodiscard]] const DEV_BROADCAST_HDR&             dev_broadcast_hdr() const noexcept             { return *reinterpret_cast<const DEV_BROADCAST_HDR*>(this->lparam); }
	[[nodiscard]] const DEV_BROADCAST_DEVICEINTERFACE& dev_broadcast_deviceinterface() const noexcept { return *reinterpret_cast<const DEV_BROADCAST_DEVICEINTERFACE*>(this->lparam); }
	[[nodiscard]] const DEV_BROADCAST_HANDLE&          dev_broadcast_handle() const noexcept          { return *reinterpret_cast<const DEV_BROADCAST_HANDLE*>(this->lparam); }
	[[nodiscard]] const DEV_BROADCAST_OEM&             dev_broadcast_oem() const noexcept             { return *reinterpret_cast<const DEV_BROADCAST_OEM*>(this->lparam); }
	[[nodiscard]] const DEV_BROADCAST_PORT&            dev_broadcast_port() const noexcept            { return *reinterpret_cast<const DEV_BROADCAST_PORT*>(this->lparam); }
	[[nodiscard]] const DEV_BROADCAST_VOLUME&          dev_broadcast_volume() const noexcept          { return *reinterpret_cast<const DEV_BROADCAST_VOLUME*>(this->lparam); }
};
#endif

struct wm_display_change : public wm {
	wm_display_change(const wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT bits_per_pixel() const noexcept { return static_cast<UINT>(this->wparam); }
	[[nodiscard]] SIZE sz() const noexcept             { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

WINLAMB_PARM_EMPTY(wm_draw_clipboard)

struct wm_draw_item : public wm {
	wm_draw_item(wm p) noexcept : wm{p} { }
	[[nodiscard]] int                   control_id() const noexcept     { return static_cast<int>(this->wparam); }
	[[nodiscard]] bool                  is_from_menu() const noexcept   { return this->control_id() == 0; }
	[[nodiscard]] const DRAWITEMSTRUCT& drawitemstruct() const noexcept { return *reinterpret_cast<const DRAWITEMSTRUCT*>(this->lparam); }
};

struct wm_drop_files : public wm {
	wm_drop_files(wm p) noexcept : wm{p} { }
	[[nodiscard]] HDROP hdrop() const noexcept { return reinterpret_cast<HDROP>(this->wparam); }
	[[nodiscard]] UINT  count() const noexcept { return DragQueryFileW(this->hdrop(), 0xffff'ffff, nullptr, 0); }

	// Retrieves all files with DragQueryFile() and calls DragFinish().
	[[nodiscard]] std::vector<std::wstring> files() const
	{
		std::vector<std::wstring> files(this->count()); // return vector, sized
		for (size_t i = 0; i < files.size(); ++i) {
			files[i].resize( // alloc path string
				static_cast<size_t>(DragQueryFileW(this->hdrop(), static_cast<UINT>(i), nullptr, 0)) + 1,
				L'\0');
			DragQueryFileW(this->hdrop(), static_cast<UINT>(i), &files[i][0],
				static_cast<UINT>(files[i].length()));
			files[i].resize(files[i].length() - 1); // trim null
		}
		DragFinish(this->hdrop());
		std::sort(files.begin(), files.end()); // make sure files are sorted
		return files;
	}

	[[nodiscard]] POINT pos() const noexcept
	{
		POINT pt{};
		DragQueryPoint(this->hdrop(), &pt);
		return pt;
	}
};

struct wm_enable : public wm {
	wm_enable(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool has_been_enabled() const noexcept { return this->wparam != FALSE; }
};

struct wm_end_session : public wm {
	wm_end_session(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_session_being_ended() const noexcept { return this->wparam != FALSE; }
	[[nodiscard]] bool is_system_issue() const noexcept        { return (this->lparam & ENDSESSION_CLOSEAPP) != 0; }
	[[nodiscard]] bool is_forced_critical() const noexcept     { return (this->lparam & ENDSESSION_CRITICAL) != 0; }
	[[nodiscard]] bool is_logoff() const noexcept              { return (this->lparam & ENDSESSION_LOGOFF) != 0; }
	[[nodiscard]] bool is_shutdown() const noexcept            { return this->lparam == 0; }
};

struct wm_enter_idle : public wm {
	wm_enter_idle(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_menu_displayed() const noexcept { return this->wparam == MSGF_MENU; }
	[[nodiscard]] HWND hwnd() const noexcept              { return reinterpret_cast<HWND>(this->lparam); }
};

struct wm_enter_menu_loop : public wm {
	wm_enter_menu_loop(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool uses_trackpopupmenu() const noexcept { return this->wparam != FALSE; }
};

WINLAMB_PARM_EMPTY(wm_enter_size_move)

struct wm_erase_bkgnd : public wm {
	wm_erase_bkgnd(wm p) noexcept : wm{p} { }
	[[nodiscard]] HDC hdc() const noexcept { return reinterpret_cast<HDC>(this->wparam); }
};

struct wm_exit_menu_loop : public wm {
	wm_exit_menu_loop(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_shortcut_menu() const noexcept { return this->wparam != FALSE; }
};

WINLAMB_PARM_EMPTY(wm_exit_size_move)

WINLAMB_PARM_EMPTY(wm_font_change)

struct wm_get_dlg_code : public wm {
	wm_get_dlg_code(wm p) noexcept : wm{p} { }
	[[nodiscard]] BYTE vkey_code() const noexcept { return static_cast<BYTE>(this->wparam); }
	[[nodiscard]] bool is_query() const noexcept  { return this->lparam == 0; }
	[[nodiscard]] const MSG* msg() const noexcept { return this->is_query() ? nullptr : reinterpret_cast<const MSG*>(this->lparam); }
	[[nodiscard]] bool has_alt() const noexcept   { return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0; }
	[[nodiscard]] bool has_ctrl() const noexcept  { return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0; }
	[[nodiscard]] bool has_shift() const noexcept { return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; }
};

WINLAMB_PARM_EMPTY(wm_get_font)

WINLAMB_PARM_EMPTY(wm_get_hot_key)

struct wm_get_icon : public wm {
	wm_get_icon(wm p) noexcept : wm{p} { }
	[[nodiscard]] BYTE icon_type() const noexcept    { return static_cast<BYTE>(this->wparam); }
	[[nodiscard]] bool is_big() const noexcept       { return this->icon_type() == ICON_BIG; }
	[[nodiscard]] bool is_small() const noexcept     { return this->icon_type() == ICON_SMALL; }
	[[nodiscard]] bool is_small_app() const noexcept { return this->icon_type() == ICON_SMALL2; }
	[[nodiscard]] UINT dpi() const noexcept          { return static_cast<UINT>(this->lparam); }
};

struct wm_get_min_max_info : public wm {
	wm_get_min_max_info(wm p) noexcept : wm{p} { }
	[[nodiscard]] MINMAXINFO& minmaxinfo() const noexcept { return *reinterpret_cast<MINMAXINFO*>(this->lparam); }
};

struct wm_get_title_bar_info_ex : public wm {
	wm_get_title_bar_info_ex(wm p) noexcept : wm{p} { }
	[[nodiscard]] const TITLEBARINFOEX& titlebarinfoex() const noexcept { return *reinterpret_cast<const TITLEBARINFOEX*>(this->lparam); }
};

struct wm_get_text : public wm {
	wm_get_text(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT     buffer_size() const noexcept { return static_cast<UINT>(this->wparam); }
	[[nodiscard]] wchar_t* buffer() const noexcept      { return reinterpret_cast<wchar_t*>(this->lparam); }
};

WINLAMB_PARM_EMPTY(wm_get_text_length)

struct wm_h_scroll : public wm {
	wm_h_scroll(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD scroll_request() const noexcept { return LOWORD(this->wparam); }
	[[nodiscard]] WORD scroll_pos() const noexcept     { return HIWORD(this->wparam); }
	[[nodiscard]] HWND hscrollbar() const noexcept     { return reinterpret_cast<HWND>(this->lparam); }
};

WINLAMB_PARM_INHERIT(wm_h_scroll_clipboard, wm_h_scroll)

struct wm_help : public wm {
	wm_help(wm p) noexcept : wm{p} { }
	[[nodiscard]] const HELPINFO& helpinfo() const noexcept { return *reinterpret_cast<const HELPINFO*>(this->lparam); }
};

struct wm_hot_key : public wm {
	wm_hot_key(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_snap_desktop() const noexcept { return this->wparam == IDHOT_SNAPDESKTOP; }
	[[nodiscard]] bool is_snap_window() const noexcept  { return this->wparam == IDHOT_SNAPWINDOW; }
	[[nodiscard]] bool has_alt() const noexcept         { return (LOWORD(this->lparam) & MOD_ALT) != 0; }
	[[nodiscard]] bool has_ctrl() const noexcept        { return (LOWORD(this->lparam) & MOD_CONTROL) != 0; }
	[[nodiscard]] bool has_shift() const noexcept       { return (LOWORD(this->lparam) & MOD_SHIFT) != 0; }
	[[nodiscard]] bool has_win() const noexcept         { return (LOWORD(this->lparam) & MOD_WIN) != 0; }
	[[nodiscard]] BYTE vkey_code() const noexcept       { return static_cast<BYTE>(HIWORD(this->lparam)); }
};

struct wm_init_dialog : public wm {
	wm_init_dialog(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND focused_ctrl() const noexcept { return reinterpret_cast<HWND>(this->wparam); }
};

struct wm_init_menu : public wm {
	wm_init_menu(wm p) noexcept : wm{p} { }
	[[nodiscard]] menu menu() const noexcept { return wl::menu{reinterpret_cast<HMENU>(this->wparam)}; }
};

struct wm_init_menu_popup : public wm {
	wm_init_menu_popup(wm p) noexcept : wm{p} { }
	[[nodiscard]] menu menu() const noexcept               { return wl::menu{reinterpret_cast<HMENU>(this->wparam)}; }
	[[nodiscard]] WORD menu_relative_pos() const noexcept  { return LOWORD(this->lparam); }
	[[nodiscard]] bool is_window_menu() const noexcept     { return HIWORD(this->lparam) != FALSE; }
};

struct wm_input_lang_change : public wm {
	wm_input_lang_change(wm p) noexcept : wm{p} { }
	[[nodiscard]] DWORD new_charset() const noexcept     { return static_cast<DWORD>(this->wparam); }
	[[nodiscard]] HKL   keyboard_layout() const noexcept { return reinterpret_cast<HKL>(this->lparam); }
};

struct wm_input_lang_change_request : public wm {
	wm_input_lang_change_request(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD new_input_locale() const noexcept { return static_cast<WORD>(this->wparam); }
	[[nodiscard]] HKL  keyboard_layout() const noexcept  { return reinterpret_cast<HKL>(this->lparam); }
};

struct wm_key_down : public wm {
	wm_key_down(wm p) noexcept : wm{p} { }
	[[nodiscard]] BYTE vkey_code() const noexcept           { return static_cast<BYTE>(this->wparam); }
	[[nodiscard]] WORD repeat_count() const noexcept        { return LOWORD(this->lparam); }
	[[nodiscard]] BYTE scan_code() const noexcept           { return LOBYTE(HIWORD(this->lparam)); }
	[[nodiscard]] bool is_extended_key() const noexcept     { return (this->lparam >> 24) & 1; }
	[[nodiscard]] bool context_code() const noexcept        { return (this->lparam >> 29) & 1; }
	[[nodiscard]] bool key_previously_down() const noexcept { return (this->lparam >> 30) & 1; }
	[[nodiscard]] bool transition_state() const noexcept    { return (this->lparam >> 31) & 1; }
};

WINLAMB_PARM_INHERIT(wm_key_up, wm_key_down)

struct wm_kill_focus : public wm {
	wm_kill_focus(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND focused_hwnd() const noexcept { return reinterpret_cast<HWND>(this->wparam); }
};

struct wm_l_button_dbl_clk : public wm {
	wm_l_button_dbl_clk(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD  vkeys() const noexcept         { return LOWORD(this->wparam); }
	[[nodiscard]] bool  has_ctrl() const noexcept      { return (this->vkeys() & MK_CONTROL) != 0; }
	[[nodiscard]] bool  has_shift() const noexcept     { return (this->vkeys() & MK_SHIFT) != 0; }
	[[nodiscard]] bool  is_left_btn() const noexcept   { return (this->vkeys() & MK_LBUTTON) != 0; }
	[[nodiscard]] bool  is_middle_btn() const noexcept { return (this->vkeys() & MK_MBUTTON) != 0; }
	[[nodiscard]] bool  is_right_btn() const noexcept  { return (this->vkeys() & MK_RBUTTON) != 0; }
	[[nodiscard]] bool  is_xbtn1() const noexcept      { return (this->vkeys() & MK_XBUTTON1) != 0; }
	[[nodiscard]] bool  is_xbtn2() const noexcept      { return (this->vkeys() & MK_XBUTTON2) != 0; }
	[[nodiscard]] POINT pos() const noexcept           { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

WINLAMB_PARM_INHERIT(wm_l_button_down, wm_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_l_button_up, wm_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_m_button_dbl_clk, wm_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_m_button_down, wm_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_m_button_up, wm_l_button_dbl_clk)

struct wm_mdi_activate : public wm {
	wm_mdi_activate(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND activated_child() const noexcept   { return reinterpret_cast<HWND>(this->wparam); }
	[[nodiscard]] HWND deactivated_child() const noexcept { return reinterpret_cast<HWND>(this->lparam); }
};

struct wm_measure_item : public wm {
	wm_measure_item(wm p) noexcept : wm{p} { }
	[[nodiscard]] const MEASUREITEMSTRUCT& measureitemstruct() const noexcept { return *reinterpret_cast<const MEASUREITEMSTRUCT*>(this->lparam); }
};

struct wm_menu_char : public wm {
	wm_menu_char(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD char_code() const noexcept      { return LOWORD(this->wparam); }
	[[nodiscard]] bool is_window_menu() const noexcept { return HIWORD(this->wparam) == MF_SYSMENU; }
	[[nodiscard]] menu menu() const noexcept           { return wl::menu{reinterpret_cast<HMENU>(this->lparam)}; }
};

struct wm_menu_drag : public wm {
	wm_menu_drag(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT initial_pos() const noexcept { return static_cast<UINT>(this->wparam); }
	[[nodiscard]] menu menu() const noexcept        { return wl::menu{reinterpret_cast<HMENU>(this->lparam)}; }
};

struct wm_menu_get_object : public wm {
	wm_menu_get_object(wm p) noexcept : wm{p} { }
	[[nodiscard]] MENUGETOBJECTINFO& menugetobjectinfo() const noexcept { return *reinterpret_cast<MENUGETOBJECTINFO*>(this->lparam); }
};

struct wm_menu_r_button_up : public wm {
	wm_menu_r_button_up(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT index() const noexcept { return static_cast<UINT>(this->wparam); }
	[[nodiscard]] menu menu() const noexcept  { return wl::menu{reinterpret_cast<HMENU>(this->lparam)}; }
};

struct wm_menu_select : public wm {
	wm_menu_select(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD item() const noexcept               { return LOWORD(this->wparam); }
	[[nodiscard]] WORD flags() const noexcept              { return HIWORD(this->wparam); }
	[[nodiscard]] bool system_closed_menu() const noexcept { return HIWORD(this->wparam) == 0xffff && !this->lparam; }
};

struct wm_mouse_activate : public wm {
	wm_mouse_activate(wm p) noexcept : wm{p} { }
	[[nodiscard]] short hit_test_code() const noexcept { return static_cast<short>(LOWORD(this->lparam)); }
	[[nodiscard]] WORD  mouse_msg_id() const noexcept  { return HIWORD(this->lparam); }
};

WINLAMB_PARM_INHERIT(wm_mouse_hover, wm_l_button_dbl_clk)

struct wm_mouse_h_wheel : public wm {
	wm_mouse_h_wheel(wm p) noexcept : wm{p} { }
	[[nodiscard]] short wheel_delta() const noexcept   { return GET_WHEEL_DELTA_WPARAM(this->wparam); }
	[[nodiscard]] WORD  vkeys() const noexcept         { return LOWORD(this->wparam); }
	[[nodiscard]] bool  has_ctrl() const noexcept      { return (this->vkeys() & MK_CONTROL) != 0; }
	[[nodiscard]] bool  has_shift() const noexcept     { return (this->vkeys() & MK_SHIFT) != 0; }
	[[nodiscard]] bool  is_left_btn() const noexcept   { return (this->vkeys() & MK_LBUTTON) != 0; }
	[[nodiscard]] bool  is_middle_btn() const noexcept { return (this->vkeys() & MK_MBUTTON) != 0; }
	[[nodiscard]] bool  is_right_btn() const noexcept  { return (this->vkeys() & MK_RBUTTON) != 0; }
	[[nodiscard]] bool  is_xbtn1() const noexcept      { return (this->vkeys() & MK_XBUTTON1) != 0; }
	[[nodiscard]] bool  is_xbtn2() const noexcept      { return (this->vkeys() & MK_XBUTTON2) != 0; }
	[[nodiscard]] POINT pos() const noexcept           { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

WINLAMB_PARM_EMPTY(wm_mouse_leave)

WINLAMB_PARM_INHERIT(wm_mouse_move, wm_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_mouse_wheel, wm_mouse_h_wheel)

struct wm_move : public wm {
	wm_move(wm p) noexcept : wm{p} { }
	[[nodiscard]] POINT client_area_pos() const noexcept { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

struct wm_moving : public wm {
	wm_moving(wm p) noexcept : wm{p} { }
	[[nodiscard]] RECT& window_pos() const noexcept { return *reinterpret_cast<RECT*>(this->lparam); }
};

struct wm_nc_activate : public wm {
	wm_nc_activate(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_active() const noexcept { return this->wparam == TRUE; }
};

struct wm_nc_calc_size : public wm {
	wm_nc_calc_size(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool                     is_nccalcsize() const noexcept     { return this->wparam == TRUE; }
	[[nodiscard]] bool                     is_rect() const noexcept           { return this->wparam == FALSE; }
	[[nodiscard]] const NCCALCSIZE_PARAMS& nccalcsize_params() const noexcept { return *reinterpret_cast<const NCCALCSIZE_PARAMS*>(this->lparam); }
	[[nodiscard]] const RECT&              rect() const noexcept              { return *reinterpret_cast<const RECT*>(this->lparam); }
};

WINLAMB_PARM_INHERIT(wm_nc_create, wm_create)

WINLAMB_PARM_EMPTY(wm_nc_destroy)

struct wm_nc_hit_test : public wm {
	wm_nc_hit_test(wm p) noexcept : wm{p} { }
	[[nodiscard]] POINT cursor_pos() const noexcept { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

struct wm_nc_l_button_dbl_clk : public wm {
	wm_nc_l_button_dbl_clk(wm p) noexcept : wm{p} { }
	[[nodiscard]] short hit_test_code() const noexcept { return static_cast<short>(this->wparam); }
	[[nodiscard]] POINT cursor_pos() const noexcept    { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

WINLAMB_PARM_INHERIT(wm_nc_l_button_down, wm_nc_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_nc_l_button_up, wm_nc_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_nc_m_button_dbl_clk, wm_nc_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_nc_m_button_down, wm_nc_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_nc_m_button_up, wm_nc_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_nc_mouse_hover, wm_nc_l_button_dbl_clk)

WINLAMB_PARM_EMPTY(wm_nc_mouse_leave)

WINLAMB_PARM_INHERIT(wm_nc_mouse_move, wm_nc_l_button_dbl_clk)

struct wm_nc_paint : public wm {
	wm_nc_paint(wm p) noexcept : wm{p} { }
	[[nodiscard]] HRGN updated_region() const noexcept { return reinterpret_cast<HRGN>(this->wparam); }
};

WINLAMB_PARM_INHERIT(wm_nc_r_button_dbl_clk, wm_nc_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_nc_r_button_down, wm_nc_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_nc_r_button_up, wm_nc_l_button_dbl_clk)

struct wm_nc_x_button_dbl_clk : public wm {
	wm_nc_x_button_dbl_clk(wm p) noexcept : wm{p} { }
	[[nodiscard]] short hit_test_code() const noexcept { return LOWORD(this->wparam); }
	[[nodiscard]] bool  is_xbtn1() const noexcept      { return HIWORD(this->wparam) == XBUTTON1; }
	[[nodiscard]] bool  is_xbtn2() const noexcept      { return HIWORD(this->wparam) == XBUTTON2; }
	[[nodiscard]] POINT cursor_pos() const noexcept    { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

WINLAMB_PARM_INHERIT(wm_nc_x_button_down, wm_nc_x_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_nc_x_button_up, wm_nc_x_button_dbl_clk)

struct wm_next_dlg_ctl : public wm {
	wm_next_dlg_ctl(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool has_ctrl_receiving_focus() const noexcept { return LOWORD(this->lparam) != FALSE; }
	[[nodiscard]] HWND ctrl_receiving_focus() const noexcept     { return LOWORD(this->lparam) ? reinterpret_cast<HWND>(this->wparam) : nullptr; }
	[[nodiscard]] bool focus_next() const noexcept               { return this->wparam == 0; }
};

struct wm_next_menu : public wm {
	wm_next_menu(wm p) noexcept : wm{p} { }
	[[nodiscard]] BYTE               vkey_code() const noexcept   { return static_cast<BYTE>(this->wparam); }
	[[nodiscard]] const MDINEXTMENU& mdinextmenu() const noexcept { return *reinterpret_cast<const MDINEXTMENU*>(this->lparam); }
};

struct wm_notify_format : public wm {
	wm_notify_format(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND hwnd_from() const noexcept             { return reinterpret_cast<HWND>(this->wparam); }
	[[nodiscard]] bool is_query_from_control() const noexcept { return this->lparam == NF_QUERY; }
	[[nodiscard]] bool is_requery_to_control() const noexcept { return this->lparam == NF_REQUERY; }
};

WINLAMB_PARM_EMPTY(wm_paint)

struct wm_paint_clipboard : public wm {
	wm_paint_clipboard(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND               clipboard_viewer() const noexcept { return reinterpret_cast<HWND>(this->wparam); }
	[[nodiscard]] const PAINTSTRUCT& paintstruct() const noexcept      { return *reinterpret_cast<const PAINTSTRUCT*>(this->lparam); }
};

struct wm_palette_changed : public wm {
	wm_palette_changed(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND hwnd_origin() const noexcept { return reinterpret_cast<HWND>(this->wparam); }
};

WINLAMB_PARM_INHERIT(wm_palette_is_changing, wm_palette_changed)

struct wm_parent_notify : public wm {
	wm_parent_notify(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT  event_message() const noexcept { return static_cast<UINT>(LOWORD(this->wparam)); }
	[[nodiscard]] WORD  child_id() const noexcept      { return HIWORD(this->wparam); }
	[[nodiscard]] HWND  child_hwnd() const noexcept    { return reinterpret_cast<HWND>(this->lparam); }
	[[nodiscard]] POINT pos() const noexcept           { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
	[[nodiscard]] bool  is_xbtn1() const noexcept      { return HIWORD(this->wparam) == XBUTTON1; }
	[[nodiscard]] bool  is_xbtn2() const noexcept      { return HIWORD(this->wparam) == XBUTTON2; }
	[[nodiscard]] WORD  pointer_flag() const noexcept  { return HIWORD(this->wparam); }
};

struct wm_power_broadcast : public wm {
	wm_power_broadcast(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool                          is_power_status_change() const noexcept  { return this->wparam == PBT_APMPOWERSTATUSCHANGE; }
	[[nodiscard]] bool                          is_resuming() const noexcept             { return this->wparam == PBT_APMRESUMEAUTOMATIC; }
	[[nodiscard]] bool                          is_suspending() const noexcept           { return this->wparam == PBT_APMSUSPEND; }
	[[nodiscard]] bool                          is_power_setting_change() const noexcept { return this->wparam == PBT_POWERSETTINGCHANGE; }
	[[nodiscard]] const POWERBROADCAST_SETTING& power_setting() const noexcept           { return *reinterpret_cast<const POWERBROADCAST_SETTING*>(this->lparam); }
};

struct wm_print : public wm {
	wm_print(wm p) noexcept : wm{p} { }
	[[nodiscard]] HDC  hdc() const noexcept   { return reinterpret_cast<HDC>(this->wparam); }
	[[nodiscard]] UINT flags() const noexcept { return static_cast<UINT>(this->lparam); }
};

WINLAMB_PARM_INHERIT(wm_print_client, wm_print)

WINLAMB_PARM_EMPTY(wm_query_drag_icon)

struct wm_query_end_session : public wm {
	wm_query_end_session(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool is_system_issue() const noexcept    { return (this->lparam & ENDSESSION_CLOSEAPP) != 0; }
	[[nodiscard]] bool is_forced_critical() const noexcept { return (this->lparam & ENDSESSION_CRITICAL) != 0; }
	[[nodiscard]] bool is_logoff() const noexcept          { return (this->lparam & ENDSESSION_LOGOFF) != 0; }
	[[nodiscard]] bool is_shutdown() const noexcept        { return this->lparam == 0; }
};

WINLAMB_PARM_EMPTY(wm_query_new_palette)

WINLAMB_PARM_EMPTY(wm_query_open)

WINLAMB_PARM_INHERIT(wm_r_button_dbl_clk, wm_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_r_button_down, wm_l_button_dbl_clk)

WINLAMB_PARM_INHERIT(wm_r_button_up, wm_l_button_dbl_clk)

#ifdef _RAS_H_ // Ras.h
struct wm_ras_dial_event : public wm {
	wm_ras_dial_event(wm p) : wm{p} { }
	[[nodiscard]] RASCONNSTATE rasconnstate() const noexcept { return static_cast<RASCONNSTATE>(this->wparam); }
	[[nodiscard]] DWORD        error() const noexcept        { return static_cast<DWORD>(this->lparam); }
};
#endif

WINLAMB_PARM_EMPTY(wm_render_all_formats)

struct wm_render_format : public wm {
	wm_render_format(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD clipboard_format() const noexcept { return static_cast<WORD>(this->wparam); }
};

struct wm_set_cursor : public wm {
	wm_set_cursor(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND  cursor_owner() const noexcept  { return reinterpret_cast<HWND>(this->wparam); }
	[[nodiscard]] short hit_test_code() const noexcept { return static_cast<short>(LOWORD(this->lparam)); }
	[[nodiscard]] WORD  mouse_msg_id() const noexcept  { return HIWORD(this->lparam); }
};

struct wm_set_focus : public wm {
	wm_set_focus(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND hwnd_losing_focus() const noexcept { return reinterpret_cast<HWND>(this->wparam); }
};

struct wm_set_font : public wm {
	wm_set_font(wm p) noexcept : wm{p} { }
	[[nodiscard]] HFONT hfont() const noexcept   { return reinterpret_cast<HFONT>(this->wparam); }
	[[nodiscard]] bool  should_redraw() noexcept { return LOWORD(this->lparam) != FALSE; }
};

struct wm_set_hot_key : public wm {
	wm_set_hot_key(wm p) noexcept : wm{p} { }
	[[nodiscard]] BYTE vkey_code() const noexcept    { return static_cast<BYTE>(LOWORD(this->wparam)); }
	[[nodiscard]] bool has_alt() const noexcept      { return (HIWORD(this->wparam) & HOTKEYF_ALT) != 0; }
	[[nodiscard]] bool has_ctrl() const noexcept     { return (HIWORD(this->wparam) & HOTKEYF_CONTROL) != 0; }
	[[nodiscard]] bool has_extended() const noexcept { return (HIWORD(this->wparam) & HOTKEYF_EXT) != 0; }
	[[nodiscard]] bool has_shift() const noexcept    { return (HIWORD(this->wparam) & HOTKEYF_SHIFT) != 0; }
};

struct wm_set_icon : public wm {
	wm_set_icon(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool  is_small() const noexcept   { return this->wparam == ICON_SMALL; }
	[[nodiscard]] HICON hicon() const noexcept      { return reinterpret_cast<HICON>(this->lparam); }
	[[nodiscard]] bool  is_removed() const noexcept { return this->hicon() == nullptr; }
};

struct wm_set_redraw : public wm {
	wm_set_redraw(wm p) noexcept : wm{p} { }
	[[nodiscard]] bool can_redraw() const noexcept { return this->wparam != FALSE; }
};

struct wm_set_text : public wm {
	wm_set_text(wm p) noexcept : wm{p} { }
	[[nodiscard]] const wchar_t* text() const noexcept { return reinterpret_cast<const wchar_t*>(this->lparam); }
};

struct wm_setting_change : public wm {
	wm_setting_change(wm p) noexcept : wm{p} { }
	[[nodiscard]] const wchar_t* string_id() const noexcept           { return reinterpret_cast<const wchar_t*>(this->lparam); }
	[[nodiscard]] bool           is_policy() const noexcept           { return !lstrcmpW(this->string_id(), L"Policy"); }
	[[nodiscard]] bool           is_locale() const noexcept           { return !lstrcmpW(this->string_id(), L"intl"); }
	[[nodiscard]] bool           is_environment_vars() const noexcept { return !lstrcmpW(this->string_id(), L"Environment"); }
};

struct wm_show_window : public wm {
	wm_show_window(wm p) noexcept : wm{p} { }
	[[nodiscard]] BYTE status() const noexcept                   { return static_cast<BYTE>(this->lparam); }
	[[nodiscard]] bool is_being_shown() const noexcept           { return this->wparam != FALSE; }
	[[nodiscard]] bool is_other_away() const noexcept            { return this->status() == SW_OTHERUNZOOM; }
	[[nodiscard]] bool is_other_over() const noexcept            { return this->status() == SW_OTHERZOOM; }
	[[nodiscard]] bool is_owner_being_minimized() const noexcept { return this->status() == SW_PARENTCLOSING; }
	[[nodiscard]] bool is_owner_being_restored() const noexcept  { return this->status() == SW_PARENTOPENING; }
};

struct wm_size : public wm {
	wm_size(wm p) noexcept : wm{p} { }
	[[nodiscard]] DWORD request() const noexcept            { return static_cast<DWORD>(this->wparam); }
	[[nodiscard]] bool  is_other_maximized() const noexcept { return this->request() == SIZE_MAXHIDE; }
	[[nodiscard]] bool  is_maximized() const noexcept       { return this->request() == SIZE_MAXIMIZED; }
	[[nodiscard]] bool  is_other_restored() const noexcept  { return this->request() == SIZE_MAXSHOW; }
	[[nodiscard]] bool  is_minimized() const noexcept       { return this->request() == SIZE_MINIMIZED; }
	[[nodiscard]] bool  is_restored() const noexcept        { return this->request() == SIZE_RESTORED; }
	[[nodiscard]] SIZE  client_area_size() const noexcept   { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

struct wm_size_clipboard : public wm {
	wm_size_clipboard(wm p) noexcept : wm{p} { }
	[[nodiscard]] HWND        clipboard_viewer() const noexcept { return reinterpret_cast<HWND>(this->wparam); }
	[[nodiscard]] const RECT& clipboard_rect() const noexcept   { return *reinterpret_cast<const RECT*>(this->lparam); }
};

struct wm_sizing : public wm {
	wm_sizing(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD  edge() const noexcept          { return static_cast<WORD>(this->wparam); }
	[[nodiscard]] RECT& screen_coords() const noexcept { return *reinterpret_cast<RECT*>(this->lparam); }
};

struct wm_spooler_status : public wm {
	wm_spooler_status(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT status_flag() const noexcept    { return static_cast<UINT>(this->wparam); }
	[[nodiscard]] WORD remaining_jobs() const noexcept { return LOWORD(this->lparam); }
};

struct wm_style_changed : public wm {
	wm_style_changed(wm p) noexcept : wm{p} { }
	[[nodiscard]] int                change() const noexcept      { return static_cast<int>(this->wparam); }
	[[nodiscard]] bool               is_style() const noexcept    { return (this->change() & GWL_STYLE) != 0; }
	[[nodiscard]] bool               is_ex_style() const noexcept { return (this->change() & GWL_EXSTYLE) != 0; }
	[[nodiscard]] const STYLESTRUCT& stylestruct() const noexcept { return *reinterpret_cast<const STYLESTRUCT*>(this->lparam); }
};

WINLAMB_PARM_INHERIT(wm_style_changing, wm_style_changed)

WINLAMB_PARM_EMPTY(wm_sync_paint)

WINLAMB_PARM_INHERIT(wm_sys_char, wm_char)

WINLAMB_PARM_EMPTY(wm_sys_color_change)

struct wm_sys_command : public wm {
	wm_sys_command(wm p) noexcept : wm{p} { }
	[[nodiscard]] WORD  command_type() const noexcept { return static_cast<WORD>(this->wparam); }
	[[nodiscard]] POINT cursor_pos() const noexcept          { return {LOWORD(this->lparam), HIWORD(this->lparam)}; }
};

WINLAMB_PARM_INHERIT(wm_sys_dead_char, wm_char)

WINLAMB_PARM_INHERIT(wm_sys_key_down, wm_key_down)

WINLAMB_PARM_INHERIT(wm_sys_key_up, wm_key_down)

struct wm_t_card : public wm {
	wm_t_card(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT action_id() const noexcept   { return static_cast<UINT>(this->wparam); }
	[[nodiscard]] long action_data() const noexcept { return static_cast<long>(this->lparam); }
};

WINLAMB_PARM_EMPTY(wm_time_change)

struct wm_timer : public wm {
	wm_timer(wm p) noexcept : wm{p} { }
	[[nodiscard]] UINT_PTR  timer_id() const noexcept { return static_cast<UINT_PTR>(this->wparam); }
	[[nodiscard]] TIMERPROC callback() const noexcept { return reinterpret_cast<TIMERPROC>(this->lparam); }
};

struct wm_un_init_menu_popup : public wm {
	wm_un_init_menu_popup(wm p) noexcept : wm{p} { }
	[[nodiscard]] menu menu() const noexcept    { return wl::menu{reinterpret_cast<HMENU>(this->wparam)}; }
	[[nodiscard]] WORD menu_id() const noexcept { return HIWORD(this->lparam); }
};

WINLAMB_PARM_INHERIT(wm_v_scroll, wm_h_scroll)

WINLAMB_PARM_INHERIT(wm_v_scroll_clipboard, wm_h_scroll)

struct wm_vkey_to_item : public wm {
	wm_vkey_to_item(wm p) noexcept : wm{p} { }
	[[nodiscard]] BYTE vkey_code() const noexcept         { return static_cast<BYTE>(LOWORD(this->wparam)); }
	[[nodiscard]] WORD current_caret_pos() const noexcept { return HIWORD(this->wparam); }
	[[nodiscard]] HWND hlistbox() const noexcept          { return reinterpret_cast<HWND>(this->lparam); }
};

struct wm_window_pos_changed : public wm {
	wm_window_pos_changed(wm p) noexcept : wm{p} { }
	[[nodiscard]] const WINDOWPOS& windowpos() const noexcept { return *reinterpret_cast<const WINDOWPOS*>(this->lparam); }
};

WINLAMB_PARM_INHERIT(wm_window_pos_changing, wm_window_pos_changed)

}//namespace wl::msg

#define WINLAMB_PARM_NOTIFY(msgName, hdrType, hdrName) \
	struct msgName : public wm { \
		msgName(wm p) noexcept : wm{p} { } \
		[[nodiscard]] hdrType& hdrName() const noexcept { return *reinterpret_cast<hdrType*>(this->lparam); } \
	};

namespace wl::msg {

WINLAMB_PARM_NOTIFY(wm_notify, NMHDR, nmhdr)

}//namespace wl::msg

/// Structs that extract values from wl::button notifications.
namespace wl::msg::button {

WINLAMB_PARM_NOTIFY(bcn_drop_down, NMBCDROPDOWN, nmbcdropdown)
WINLAMB_PARM_NOTIFY(bcn_hot_item_change, NMBCHOTITEM, nmbchotitem)
WINLAMB_PARM_INHERIT(bn_clicked, wm_command)
WINLAMB_PARM_INHERIT(bn_dbl_clk, wm_command)
WINLAMB_PARM_INHERIT(bn_kill_focus, wm_command)
WINLAMB_PARM_INHERIT(bn_set_focus, wm_command)
WINLAMB_PARM_NOTIFY(nm_custom_draw, NMCUSTOMDRAW, nmcustomdraw)

}//namespace wl::msg::button

/// Structs that extract values from wl::combo_box notifications.
namespace wl::msg::combo_box {

WINLAMB_PARM_INHERIT(cbn_close_up, wm_command)
WINLAMB_PARM_INHERIT(cbn_dbl_clk, wm_command)
WINLAMB_PARM_INHERIT(cbn_drop_down, wm_command)
WINLAMB_PARM_INHERIT(cbn_edit_change, wm_command)
WINLAMB_PARM_INHERIT(cbn_edit_update, wm_command)
WINLAMB_PARM_INHERIT(cbn_err_space, wm_command)
WINLAMB_PARM_INHERIT(cbn_kill_focus, wm_command)
WINLAMB_PARM_INHERIT(cbn_sel_change, wm_command)
WINLAMB_PARM_INHERIT(cbn_sel_end_cancel, wm_command)
WINLAMB_PARM_INHERIT(cbn_sel_end_ok, wm_command)
WINLAMB_PARM_INHERIT(cbn_set_focus, wm_command)

}//namespace wl::msg::combo_box

/// Structs that extract values from wl::combo_box_ex notifications.
namespace wl::msg::combo_box_ex {

WINLAMB_PARM_INHERIT(cbn_close_up, wm_command)
WINLAMB_PARM_INHERIT(cbn_dbl_clk, wm_command)
WINLAMB_PARM_INHERIT(cbn_drop_down, wm_command)
WINLAMB_PARM_INHERIT(cbn_edit_change, wm_command)
WINLAMB_PARM_INHERIT(cbn_edit_update, wm_command)
WINLAMB_PARM_INHERIT(cbn_err_space, wm_command)
WINLAMB_PARM_INHERIT(cbn_kill_focus, wm_command)
WINLAMB_PARM_INHERIT(cbn_sel_change, wm_command)
WINLAMB_PARM_INHERIT(cbn_sel_end_cancel, wm_command)
WINLAMB_PARM_INHERIT(cbn_sel_end_ok, wm_command)
WINLAMB_PARM_INHERIT(cbn_set_focus, wm_command)

WINLAMB_PARM_NOTIFY(cben_begin_edit, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(cben_delete_item, NMCOMBOBOXEXW, nmcomboboxex)
WINLAMB_PARM_NOTIFY(cben_drag_begin, NMCBEDRAGBEGINW, nmcbedragbegin)
WINLAMB_PARM_NOTIFY(cben_end_edit, NMCBEENDEDITW, nmcbeendedit)
WINLAMB_PARM_NOTIFY(cben_get_disp_info, NMCOMBOBOXEXW, nmcomboboxex)
WINLAMB_PARM_NOTIFY(cben_insert_item, NMCOMBOBOXEXW, nmcomboboxex)
WINLAMB_PARM_NOTIFY(nm_set_cursor, NMMOUSE, nmmouse)

}//namespace wl::msg::combo_box_ex

/// Structs that extract values from wl::date_time_picker notifications.
namespace wl::msg::date_time_picker {

WINLAMB_PARM_NOTIFY(dtn_close_up, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(dtn_date_time_change, NMDATETIMECHANGE, nmdatetimechange)
WINLAMB_PARM_NOTIFY(dtn_drop_down, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(dtn_format, NMDATETIMEFORMATW, nmdatetimeformat)
WINLAMB_PARM_NOTIFY(dtn_format_query, NMDATETIMEFORMATQUERYW, nmdatetimeformatquery)
WINLAMB_PARM_NOTIFY(dtn_user_string, NMDATETIMESTRINGW, nmdatetimestring)
WINLAMB_PARM_NOTIFY(dtn_wm_key_down, NMDATETIMEWMKEYDOWNW, nmdatetimewmkeydown)
WINLAMB_PARM_NOTIFY(nm_kill_focus, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_set_focus, NMHDR, nmhdr)

}//namespace wl::msg::date_time_picker

/// Structs that extract values from wl::edit notifications.
namespace wl::msg::edit {

WINLAMB_PARM_INHERIT(en_align_ltr_ec, wm_command)
WINLAMB_PARM_INHERIT(en_align_rtl_ec, wm_command)
WINLAMB_PARM_INHERIT(en_change, wm_command)
WINLAMB_PARM_INHERIT(en_err_space, wm_command)
WINLAMB_PARM_INHERIT(en_h_scroll, wm_command)
WINLAMB_PARM_INHERIT(en_kill_focus, wm_command)
WINLAMB_PARM_INHERIT(en_max_text, wm_command)
WINLAMB_PARM_INHERIT(en_set_focus, wm_command)
WINLAMB_PARM_INHERIT(en_update, wm_command)
WINLAMB_PARM_INHERIT(en_v_scroll, wm_command)

}//namespace wl::msg::edit

/// Structs that extract values from wl::header notifications.
namespace wl::msg::header {

WINLAMB_PARM_NOTIFY(hdn_begin_drag, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_begin_filter_edit, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_begin_track, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_divider_dbl_click, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_drop_down, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_end_drag, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_end_filter_edit, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_end_track, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_filter_btn_click, NMHDFILTERBTNCLICK, nmfilterbtnclick)
WINLAMB_PARM_NOTIFY(hdn_filter_change, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_get_disp_info, NMHDDISPINFOW, nmhddispinfo)
WINLAMB_PARM_NOTIFY(hdn_item_changed, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_item_changing, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_item_click, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_item_dbl_click, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_item_key_down, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_item_state_icon_click, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_overflow_click, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(hdn_track, NMHEADERW, nmheader)
WINLAMB_PARM_NOTIFY(nm_custom_draw, NMCUSTOMDRAW, nmcustomdraw)
WINLAMB_PARM_NOTIFY(nm_r_click, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_released_capture, NMHDR, nmhdr)

}//namespace wl::msg::header

/// Structs that extract values from wl::ip_addr notifications.
namespace wl::msg::ip_addr {

WINLAMB_PARM_NOTIFY(ipn_field_changed, NMIPADDRESS, nmipaddress)
WINLAMB_PARM_INHERIT(en_change, wm_command)
WINLAMB_PARM_INHERIT(en_kill_focus, wm_command)
WINLAMB_PARM_INHERIT(en_set_focus, wm_command)

}//namespace wl::msg::ip_addr

/// Structs that extract values from wl::label notifications.
namespace wl::msg::label {

WINLAMB_PARM_INHERIT(stn_clicked, wm_command)
WINLAMB_PARM_INHERIT(stn_dbl_clk, wm_command)
WINLAMB_PARM_INHERIT(stn_disable, wm_command)
WINLAMB_PARM_INHERIT(stn_enable, wm_command)

}//namespace wl::msg::label

/// Structs that extract values from wl::list_box notifications.
namespace wl::msg::list_box {

WINLAMB_PARM_INHERIT(lbn_dbl_clk, wm_command)
WINLAMB_PARM_INHERIT(lbn_err_space, wm_command)
WINLAMB_PARM_INHERIT(lbn_kill_focus, wm_command)
WINLAMB_PARM_INHERIT(lbn_sel_cancel, wm_command)
WINLAMB_PARM_INHERIT(lbn_sel_change, wm_command)
WINLAMB_PARM_INHERIT(lbn_set_focus, wm_command)

}//namespace wl::msg::list_box

/// Structs that extract values from wl::list_view notifications.
namespace wl::msg::list_view {

WINLAMB_PARM_NOTIFY(lvn_begin_drag, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_begin_label_edit, NMLVDISPINFOW, nmlvdispinfo)
WINLAMB_PARM_NOTIFY(lvn_begin_r_drag, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_begin_scroll, NMLVSCROLL, nmlvscroll)
WINLAMB_PARM_NOTIFY(lvn_column_click, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_column_drop_down, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_column_overflow_click, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_delete_all_items, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_delete_item, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_end_label_edit, NMLVDISPINFOW, nmlvdispinfo)
WINLAMB_PARM_NOTIFY(lvn_end_scroll, NMLVSCROLL, nmlvscroll)
WINLAMB_PARM_NOTIFY(lvn_get_disp_info, NMLVDISPINFOW, nmlvdispinfo)
WINLAMB_PARM_NOTIFY(lvn_get_empty_markup, NMLVEMPTYMARKUP, nmlvemptymarkup)
WINLAMB_PARM_NOTIFY(lvn_get_info_tip, LPNMLVGETINFOTIPW, nmlvgetinfotip)
WINLAMB_PARM_NOTIFY(lvn_hot_track, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_incremental_search, NMLVFINDITEMW, nmlvfinditem)
WINLAMB_PARM_NOTIFY(lvn_insert_item, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_item_activate, NMITEMACTIVATE, nmitemactivate)
WINLAMB_PARM_NOTIFY(lvn_item_changed, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_item_changing, NMLISTVIEW, nmlistview)
WINLAMB_PARM_NOTIFY(lvn_key_down, NMLVKEYDOWN, nmlvkeydown)
WINLAMB_PARM_NOTIFY(lvn_link_click, NMLVLINK, nmlvlink)
WINLAMB_PARM_NOTIFY(lvn_marquee_begin, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(lvn_od_cache_hint, NMLVCACHEHINT, nmlvcachehint)
WINLAMB_PARM_NOTIFY(lvn_od_find_item, NMLVFINDITEMW, nmlvfinditem)
WINLAMB_PARM_NOTIFY(lvn_od_state_changed, NMLVODSTATECHANGE, nmlvodstatechange)
WINLAMB_PARM_NOTIFY(lvn_set_disp_info, NMLVDISPINFOW, nmlvdispinfo)
WINLAMB_PARM_NOTIFY(nm_click, NMITEMACTIVATE, nmitemactivate)
WINLAMB_PARM_NOTIFY(nm_custom_draw, NMLVCUSTOMDRAW, nmlvcustomdraw)
WINLAMB_PARM_NOTIFY(nm_dbl_clk, NMITEMACTIVATE, nmitemactivate)
WINLAMB_PARM_NOTIFY(nm_hover, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_kill_focus, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_r_click, NMITEMACTIVATE, nmitemactivate)
WINLAMB_PARM_NOTIFY(nm_r_dbl_clk, NMITEMACTIVATE, nmitemactivate)
WINLAMB_PARM_NOTIFY(nm_released_capture, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_return, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_set_focus, NMHDR, nmhdr)

}//namespace wl::msg::list_view

/// Structs that extract values from wl::month_calendar notifications.
namespace wl::msg::month_calendar {

WINLAMB_PARM_NOTIFY(mcn_get_day_state, NMDAYSTATE, nmdaystate)
WINLAMB_PARM_NOTIFY(mcn_sel_change, NMSELCHANGE, nmselchange)
WINLAMB_PARM_NOTIFY(mcn_select, NMSELCHANGE, nmselchange)
WINLAMB_PARM_NOTIFY(mcn_view_change, NMVIEWCHANGE, nmviewchange)
WINLAMB_PARM_NOTIFY(nm_released_capture, NMHDR, nmhdr)

}//namespace wl::msg::month_calendar

/// Structs that extract values from wl::rebar notifications.
namespace wl::msg::rebar {

WINLAMB_PARM_NOTIFY(rbn_auto_break, NMREBARAUTOBREAK, nmrebarautobreak)
WINLAMB_PARM_NOTIFY(rbn_auto_size, NMRBAUTOSIZE, nmrbautosize)
WINLAMB_PARM_NOTIFY(rbn_begin_drag, NMREBAR, nmrebar)
WINLAMB_PARM_NOTIFY(rbn_chevron_pushed, NMREBARCHEVRON, nmrebarchevron)
WINLAMB_PARM_NOTIFY(rbn_child_size, NMREBARCHILDSIZE, nmrebarchildsize)
WINLAMB_PARM_NOTIFY(rbn_deleted_band, NMREBAR, nmrebar)
WINLAMB_PARM_NOTIFY(rbn_deleting_band, NMREBAR, nmrebar)
WINLAMB_PARM_NOTIFY(rbn_end_drag, NMREBAR, nmrebar)
WINLAMB_PARM_NOTIFY(rbn_get_object, NMOBJECTNOTIFY, nmobjectnotify)
WINLAMB_PARM_NOTIFY(rbn_height_change, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(rbn_layout_changed, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(rbn_min_max, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(rbn_splitter_drag, NMREBARSPLITTER, nmrebarsplitter)
WINLAMB_PARM_NOTIFY(nm_custom_draw, NMCUSTOMDRAW, nmcustomdraw)
WINLAMB_PARM_NOTIFY(nm_nc_hit_test, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_released_capture, NMHDR, nmhdr)

}//namespace wl::msg::rebar

/// Structs that extract values from wl::status_bar notifications.
namespace wl::msg::status_bar {

WINLAMB_PARM_NOTIFY(sbn_simple_mode_change, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_click, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_dbl_clk, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_r_click, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_r_dbl_clk, NMMOUSE, nmmouse)

}//namespace wl::msg::status_bar

/// Structs that extract values from wl::sys_link notifications.
namespace wl::msg::sys_link {

WINLAMB_PARM_NOTIFY(nm_click, NMLINK, nmlink)

}//namespace wl::msg::sys_link

/// Structs that extract values from wl::tab notifications.
namespace wl::msg::tab {

WINLAMB_PARM_NOTIFY(tcn_focus_change, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(tcn_get_object, NMOBJECTNOTIFY, nmobjectnotify)
WINLAMB_PARM_NOTIFY(tcn_key_down, NMTCKEYDOWN, nmtckeydown)
WINLAMB_PARM_NOTIFY(tcn_sel_change, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(tcn_sel_changing, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_click, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_dbl_clk, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_r_click, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_r_dbl_clk, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_released_capture, NMHDR, nmhdr)

}//namespace wl::msg::tab

// For some weird reason, the structs below don't exist in Windows headers.

// https://docs.microsoft.com/en-us/windows/win32/controls/tbn-dupaccelerator
typedef struct tagNMTBDUPACCELERATOR {
	NMHDR hdr;
	UINT ch;
	BOOL fDup;
} NMTBDUPACCELERATOR, *LPNMTBDUPACCELERATOR;

// https://docs.microsoft.com/en-us/windows/win32/controls/tbn-wrapaccelerator
typedef struct tagNMTBWRAPACCELERATOR {
	NMHDR hdr;
	UINT ch;
	int iButton;
} NMTBWRAPACCELERATOR, *LPNMTBWRAPACCELERATOR;

// https://docs.microsoft.com/en-us/windows/win32/controls/tbn-wraphotitem
typedef struct tagNMTBWRAPHOTITEM {
	NMHDR hdr;
	int iStart;
	int iDir;
	UINT nReason;
} NMTBWRAPHOTITEM, *LPNMTBWRAPHOTITEM;

/// Structs that extract values from wl::tool_bar notifications.
namespace wl::msg::tool_bar {

WINLAMB_PARM_NOTIFY(tbn_begin_adjust, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(tbn_begin_drag, NMTOOLBARW, nmtoolbar)
WINLAMB_PARM_NOTIFY(tbn_cust_help, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(tbn_deleting_button, NMTOOLBARW, nmtoolbar)
WINLAMB_PARM_NOTIFY(tbn_drag_out, NMTOOLBARW, nmtoolbar)
WINLAMB_PARM_NOTIFY(tbn_drag_over, NMTBHOTITEM, nmtbhotitem)
WINLAMB_PARM_NOTIFY(tbn_drop_down, NMTOOLBARW, nmtoolbar)
WINLAMB_PARM_NOTIFY(tbn_dup_accelerator, NMTBDUPACCELERATOR, nmtbdupaccelerator)
WINLAMB_PARM_NOTIFY(tbn_end_adjust, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(tbn_end_drag, NMTOOLBARW, nmtoolbar)
WINLAMB_PARM_NOTIFY(tbn_get_button_info, NMTOOLBARW, nmtoolbar)
WINLAMB_PARM_NOTIFY(tbn_get_disp_info, NMTBDISPINFOW, nmtbdispinfo)
WINLAMB_PARM_NOTIFY(tbn_get_info_tip, NMTBGETINFOTIPW, nmtbgetinfotip)
WINLAMB_PARM_NOTIFY(tbn_get_object, NMOBJECTNOTIFY, nmobjectnotify)
WINLAMB_PARM_NOTIFY(tbn_hot_item_change, NMTBHOTITEM, nmtbhotitem)
WINLAMB_PARM_NOTIFY(tbn_init_customize, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(tbn_map_accelerator, NMCHAR, nmchar)
WINLAMB_PARM_NOTIFY(tbn_query_delete, NMTOOLBARW, nmtoolbar)
WINLAMB_PARM_NOTIFY(tbn_query_insert, NMTOOLBARW, nmtoolbar)
WINLAMB_PARM_NOTIFY(tbn_reset, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(tbn_restore, NMTBRESTORE, nmtbrestore)
WINLAMB_PARM_NOTIFY(tbn_save, NMTBSAVE, nmtbsave)
WINLAMB_PARM_NOTIFY(tbn_toolbar_change, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(tbn_wrap_accelerator, NMTBWRAPACCELERATOR, nmtbwrapaccelerator)
WINLAMB_PARM_NOTIFY(tbn_wrap_hot_item, NMTBWRAPHOTITEM, nmtbwraphotitem)
WINLAMB_PARM_NOTIFY(nm_char, NMCHAR, nmchar)
WINLAMB_PARM_NOTIFY(nm_click, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_custom_draw, NMCUSTOMDRAW, nmcustomdraw)
WINLAMB_PARM_NOTIFY(nm_dbl_clk, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_key_down, NMKEY, nmkey)
WINLAMB_PARM_NOTIFY(nm_l_down, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_r_click, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_r_dbl_clk, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_released_capture, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_tooltips_created, NMTOOLTIPSCREATED, nmtooltipscreated)

}//namespace wl::msg::tool_bar

/// Structs that extract values from wl::track_bar notifications.
namespace wl::msg::track_bar {

WINLAMB_PARM_NOTIFY(trbn_thumb_pos_changing, NMTRBTHUMBPOSCHANGING, nmtrbthumbposchanging)
WINLAMB_PARM_NOTIFY(nm_custom_draw, NMCUSTOMDRAW, nmcustomdraw)
WINLAMB_PARM_NOTIFY(nm_released_capture, NMHDR, nmhdr)

}//namespace wl::msg::track_bar

/// Structs that extract values from wl::tooltip notifications.
namespace wl::msg::tooltip {

WINLAMB_PARM_NOTIFY(ttn_get_disp_info, NMTTDISPINFOW, nmttdispinfo)
WINLAMB_PARM_NOTIFY(ttn_link_click, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(ttn_need_text, NMTTDISPINFOW, nmttdispinfo)
WINLAMB_PARM_NOTIFY(ttn_pop, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(ttn_show, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_custom_draw, NMTTCUSTOMDRAW, nmttcustomdraw)

}//namespace wl::msg::tooltip

/// Structs that extract values from wl::tree_view notifications.
namespace wl::msg::tree_view {

WINLAMB_PARM_NOTIFY(tvn_async_draw, NMTVASYNCDRAW, nmtvasyncdraw)
WINLAMB_PARM_NOTIFY(tvn_begin_drag, NMTREEVIEWW, nmtreeview)
WINLAMB_PARM_NOTIFY(tvn_begin_label_edit, NMTVDISPINFOW, nmtvdispinfo)
WINLAMB_PARM_NOTIFY(tvn_begin_r_drag, NMTREEVIEWW, nmtreeview)
WINLAMB_PARM_NOTIFY(tvn_delete_item, NMTREEVIEWW, nmtreeview)
WINLAMB_PARM_NOTIFY(tvn_end_label_edit, NMTVDISPINFOW, nmtvdispinfo)
WINLAMB_PARM_NOTIFY(tvn_get_disp_info, NMTVDISPINFOW, nmtvdispinfo)
WINLAMB_PARM_NOTIFY(tvn_get_info_tip, NMTVGETINFOTIPW, nmtvgetinfotip)
WINLAMB_PARM_NOTIFY(tvn_item_changed, NMTVITEMCHANGE, nmtvitemchange)
WINLAMB_PARM_NOTIFY(tvn_item_changing, NMTVITEMCHANGE, nmtvitemchange)
WINLAMB_PARM_NOTIFY(tvn_item_expanded, NMTREEVIEWW, nmtreeview)
WINLAMB_PARM_NOTIFY(tvn_item_expanding, NMTREEVIEWW, nmtreeview)
WINLAMB_PARM_NOTIFY(tvn_key_down, NMTVKEYDOWN, nmtvkeydown)
WINLAMB_PARM_NOTIFY(tvn_sel_changed, NMTREEVIEWW, nmtreeview)
WINLAMB_PARM_NOTIFY(tvn_sel_changing, NMTREEVIEWW, nmtreeview)
WINLAMB_PARM_NOTIFY(tvn_set_disp_info, NMTVDISPINFOW, nmtvdispinfo)
WINLAMB_PARM_NOTIFY(tvn_single_expand, NMTREEVIEWW, nmtreeview)
WINLAMB_PARM_NOTIFY(nm_click, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_custom_draw, NMTVCUSTOMDRAW, nmtvcustomdraw)
WINLAMB_PARM_NOTIFY(nm_dbl_clk, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_kill_focus, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_r_click, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_r_dbl_clk, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_return, NMHDR, nmhdr)
WINLAMB_PARM_NOTIFY(nm_set_cursor, NMMOUSE, nmmouse)
WINLAMB_PARM_NOTIFY(nm_set_focus, NMHDR, nmhdr)

}//namespace wl::msg::tree_view

/// Structs that extract values from wl::up_down notifications.
namespace wl::msg::up_down {

WINLAMB_PARM_NOTIFY(udn_delta_pos, NMUPDOWN, nmupdown)
WINLAMB_PARM_NOTIFY(nm_released_capture, NMHDR, nmhdr)

}//namespace wl::msg::up_down

#undef WINLAMB_PARM_NOTIFY
#undef WINLAMB_PARM_INHERIT
#undef WINLAMB_PARM_EMPTY