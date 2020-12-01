/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <functional>
#include <Windows.h>
#include <CommCtrl.h>
#include "lambda_type.h"
#include "msg_wm.h"

namespace _wli {

// Subclasses the parent window and, during the given message, executes a lambda.
// Used by native controls.
template<UINT M, typename P>
class base_parent_subclass final {
private:
	inline static UINT_PTR _baseSubclassId = 0;
	std::function<void(P)> _action;

public:
	template<typename F>
	auto subclass(HWND hParent, F&& action)
		-> WINLAMB_LAMBDA_TYPE(action, void(P), void)
	{
		this->_action = std::forward<F>(action);

		if (SetWindowSubclass(hParent, _subclass_proc, ++this->_baseSubclassId,
			reinterpret_cast<DWORD_PTR>(this)) == FALSE) // pass pointer to self
		{
			throw std::runtime_error("Installing resizer subclass failed in " __FUNCTION__ "().");
		}
	}

private:
	static LRESULT CALLBACK _subclass_proc(HWND hWnd, UINT msg,
		WPARAM wp, LPARAM lp, UINT_PTR idSubclass, DWORD_PTR refData) noexcept
	{
		base_parent_subclass* pSelf = nullptr;
		bool processed = false;

		if (msg == M) {
			pSelf = reinterpret_cast<base_parent_subclass*>(refData);
			if (pSelf != nullptr && pSelf->_action != nullptr) {
				pSelf->_action(P{wl::msg::wm{wp, lp}}); // execute user function
				processed = true;
			}
		} else if (msg == WM_NCDESTROY) { // always check
			RemoveWindowSubclass(hWnd, _subclass_proc, idSubclass);
		}

		return DefSubclassProc(hWnd, msg, wp, lp);
	}
};

}//namespace _wli