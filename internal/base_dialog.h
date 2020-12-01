/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <stdexcept>
#include <system_error>
#include <Windows.h>
#include "base_msg_handler.h"
#include "gdi_obj.h"
#include "interfaces.h"

namespace _wli {

// Owns the HWND.
// Calls CreateDialogParam() or DialogBoxParam().
// Provides the dialog procedure.
class base_dialog final {
private:
	HWND _hWnd = nullptr;
	base_msg_handler _msgHandler{true}; // true = is a dialog
	bool _centerOnParent = false;

public:
	~base_dialog()
	{
		if (this->_hWnd != nullptr) {
			SetWindowLongPtrW(this->_hWnd, DWLP_USER, 0); // clear passed pointer
		}
	}

	[[nodiscard]] const HWND& hwnd() const noexcept { return this->_hWnd; } // note: returning a const reference
	
	void center_on_parent() noexcept { this->_centerOnParent = true; } // for modal dialogs

	void create_children(
		std::initializer_list<std::reference_wrapper<wl::i_resource_control>> children)
	{
		for (std::reference_wrapper<wl::i_resource_control> child : children) {
			child.get().create_in_dialog();
		}
	}

	template<typename F>
	void background_work(F&& func) { this->_msgHandler.background_work(this->_hWnd, std::move(func)); }

	[[nodiscard]] wl::msg::wnd_events_all& on()
	{
		if (this->_hWnd != nullptr) {
			throw std::logic_error("Cannot add a message handler after the dialog was created.");
		}
		return this->_msgHandler.on();
	}

	HWND create_dialog_param(HINSTANCE hInst, const wl::i_window* parent, int dialogId)
	{
		this->_creation_checks(dialogId);

		HWND h = CreateDialogParamW(hInst, MAKEINTRESOURCEW(dialogId),
			parent != nullptr ? parent->hwnd() : nullptr,
			_dialog_proc, reinterpret_cast<LPARAM>(this)); // pass pointer to self

		if (h == nullptr) {
			throw std::system_error(GetLastError(), std::system_category(),
				"CreateDialogParam() failed in base_dialog::create_dialog_param().");
		}
		return h; // our _hWnd member is set during WM_INITDIALOG processing
	}

	INT_PTR dialog_box_param(HINSTANCE hInst, const wl::i_window* parent, int dialogId)
	{
		this->_creation_checks(dialogId);

		// This function blocks until the dialog is closed.
		INT_PTR ret = DialogBoxParamW(hInst, MAKEINTRESOURCEW(dialogId),
			parent != nullptr ? parent->hwnd() : nullptr,
			_dialog_proc, reinterpret_cast<LPARAM>(this)); // pass pointer to self

		DWORD lerr = GetLastError();
		if (lerr == -1) {
			throw std::system_error(lerr, std::system_category(),
				"DialogBoxParam() failed in base_dialog::dialog_box_param().");
		}
		return ret;
	}

private:
	void _creation_checks(int dialogId)
	{
		if (this->_hWnd != nullptr) {
			throw std::logic_error("Cannot create a dialog twice.");
		} else if (dialogId == 0) {
			throw std::logic_error("No dialog resource ID given on dialog setup.");
		}
	}

	static INT_PTR CALLBACK _dialog_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) noexcept
	{
		base_dialog* pSelf = nullptr;

		if (msg == WM_INITDIALOG) {
			pSelf = reinterpret_cast<base_dialog*>(lp);
			SetWindowLongPtrW(hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(pSelf)); // store
			if (pSelf->_centerOnParent) {
				_center_on_parent(hWnd);
			}
			_set_ui_font_on_children(hWnd);
			pSelf->_hWnd = hWnd; // store HWND in class member
		} else {
			pSelf = reinterpret_cast<base_dialog*>(GetWindowLongPtrW(hWnd, DWLP_USER)); // retrieve
		}

		// If no pointer stored, then no processing is done.
		// Prevents processing before WM_INITDIALOG and after WM_NCDESTROY.
		if (pSelf == nullptr) {
			return FALSE;
		}

		// Execute user handler, if any.
		std::optional<LRESULT> ret = pSelf->_msgHandler.exec(msg, wp, lp);

		if (msg == WM_NCDESTROY) {
			SetWindowLongPtrW(hWnd, DWLP_USER, 0); // clear passed pointer
			pSelf->_hWnd = nullptr; // clear stored HWND
		}
		return ret.value_or(FALSE);
	}

	static void _center_on_parent(HWND hDlg) noexcept
	{
		RECT rc{}, rcParent{};
		GetWindowRect(hDlg, &rc);
		GetWindowRect(GetParent(hDlg), &rcParent);
		SetWindowPos(hDlg, nullptr,
			rcParent.left + ((rcParent.right - rcParent.left) / 2) - (rc.right - rc.left) / 2,
			rcParent.top + ((rcParent.bottom - rcParent.top) / 2) - (rc.bottom - rc.top) / 2,
			0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	static void _set_ui_font_on_children(HWND hDlg) noexcept
	{
		SendMessageW(hDlg, WM_SETFONT,
			reinterpret_cast<WPARAM>(globalUiFont.hfont()), MAKELPARAM(FALSE, 0));
		EnumChildWindows(hDlg, [](HWND hWnd, LPARAM lp) noexcept -> BOOL {
			SendMessageW(hWnd, WM_SETFONT,
				reinterpret_cast<WPARAM>(reinterpret_cast<HFONT>(lp)),
				MAKELPARAM(FALSE, 0)); // will run on each child
			return TRUE;
		}, reinterpret_cast<LPARAM>(globalUiFont.hfont()));
	}
};

}//namespace _wli