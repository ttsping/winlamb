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
#include <CommCtrl.h>
#include "depot.h"
#include "interfaces.h"
#include "msg_wnd_events.h"
#include "run.h"

namespace _wli {

inline WORD next_auto_ctrl_id()
{
	static WORD autoCtrlId = 20001; // intermediary number between Visual Studio Resource Editor bases
	return autoCtrlId++; // auto-generate a control ID
}

// Owns the HWND.
// Keeps the subclass handlers.
// Provides the subclass procedure.
class base_native_control final {
private:
	HWND _hWnd = nullptr;
	wl::i_parent_window& _parent; // non-const because of wm_events, which has a reference to us, and calls parent.on()
	bool _parentIsDlg = false; // set during create_window() methods
	WORD _ctrlId;
	UINT_PTR _subclassId = 0;
	depot<UINT> _depotWmSubcls; // WM messages
	depot<UINT_PTR> _depotTimerSubcls; // WM_TIMER
	wl::msg::wnd_events _wndEventsSubcls{_depotWmSubcls, _depotTimerSubcls, false}; // false = not a dialog

public:
	base_native_control(wl::i_parent_window& parent, WORD ctrlId) noexcept
		: _parent{parent}, _ctrlId{ctrlId} { }

	explicit base_native_control(wl::i_parent_window& parent) noexcept
		: base_native_control{parent, next_auto_ctrl_id()} { }

	base_native_control(base_native_control&&) = default;
	base_native_control& operator=(base_native_control&&) = default;

	[[nodiscard]] HWND hwnd() const noexcept { return this->_hWnd; }
	[[nodiscard]] WORD ctrl_id() const noexcept { return this->_ctrlId; }
	[[nodiscard]] wl::i_parent_window& parent() noexcept { return this->_parent; }
	[[nodiscard]] bool parent_is_dlg() const noexcept { return this->_parentIsDlg; }

	// Calls CreateWindowEx().
	void create_window(std::wstring_view className, std::optional<std::wstring_view> title,
		POINT pos, SIZE size, DWORD styles, DWORD exStyles)
	{
		this->_set_parent_is_dialog();

		this->_hWnd = CreateWindowExW(exStyles, className.data(),
			title.has_value() ? title.value().data() : nullptr,
			styles, pos.x, pos.y, size.cx, size.cy, this->_parent.hwnd(),
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(this->_ctrlId)),
			reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(this->_parent.hwnd(), GWLP_HINSTANCE)),
			nullptr);

		if (this->_hWnd == nullptr) {
			throw std::system_error(GetLastError(), std::system_category(),
				"CreateWindowEx() failed in base_native_control::create_window().");
		}

		this->_install_subclass_if_needed();
	}

	// To be used on dialogs.
	void create_window()
	{
		this->_set_parent_is_dialog();

		if (!this->_parentIsDlg) {
			throw std::logic_error("Parent is not a dialog, you called the wrong create() on the control.");
		}

		this->_hWnd = GetDlgItem(this->_parent.hwnd(), this->_ctrlId);
		this->_install_subclass_if_needed();
	}

	[[nodiscard]] wl::msg::wnd_events& on_subclass()
	{
		if (this->_hWnd != nullptr) {
			throw std::logic_error("Cannot add a subclass handler after the native control was created.");
		}
		return this->_wndEventsSubcls;
	}

private:
	void _set_parent_is_dialog()
	{
		if (this->_hWnd != nullptr) {
			throw std::logic_error("Cannot create a control twice.");
		} else if (this->_parent.hwnd() == nullptr) {
			throw std::logic_error("Cannot create a control before its parent is created.");
		}

		// https://stackoverflow.com/a/64437627/6923555
		this->_parentIsDlg = MAKEINTATOM(GetClassLongPtrW(this->_parent.hwnd(), GCW_ATOM)) == WC_DIALOG;
	}

	void _install_subclass_if_needed()
	{
		// At least 1 subclass message handler was added?
		if (!this->_depotWmSubcls.empty() || !this->_depotTimerSubcls.empty()) {
			static UINT_PTR firstId = 0;
			this->_subclassId = ++firstId;

			if (SetWindowSubclass(this->_hWnd, _subclass_proc, this->_subclassId,
				reinterpret_cast<DWORD_PTR>(this)) == FALSE) // pass pointer to self
			{
				throw std::runtime_error("Installing native control subclass failed in " __FUNCTION__ "().");
			}
		}
	}

	static LRESULT CALLBACK _subclass_proc(HWND hWnd, UINT msg,
		WPARAM wp, LPARAM lp, UINT_PTR idSubclass, DWORD_PTR refData) noexcept
	{
		base_native_control* pSelf = reinterpret_cast<base_native_control*>(refData);
		std::function<LRESULT(wl::msg::wm)>* userFunc = nullptr;
		LRESULT retVal = 0;

		if (pSelf != nullptr && pSelf->_hWnd != nullptr) {
			userFunc = (msg == WM_TIMER)
				? pSelf->_depotTimerSubcls.find(static_cast<UINT_PTR>(wp))
				: pSelf->_depotWmSubcls.find(msg);

			if (userFunc != nullptr) {
				run::catch_all_exceptions([&retVal, userFunc, wp, lp]() {
					retVal = (*userFunc)({wp, lp}); // execute user lambda
				}, run::post_quit_on_catch::YES);
			}
		}

		if (msg == WM_NCDESTROY) { // always check
			RemoveWindowSubclass(hWnd, _subclass_proc, idSubclass);
		}
		return userFunc != nullptr
			? retVal
			: DefSubclassProc(hWnd, msg, wp, lp);
	}
};

}//namespace _wli