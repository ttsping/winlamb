/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <functional>
#include <Windows.h>
#include "lambda_type.h"

namespace _wli {

const UINT WM_UI_WORK_THREAD = WM_APP + 0x3fff;

}//namespace _wli

namespace wl {

/// Exposes a method that allows code to run in UI thread.
/// This class is used as the argument for background_work() lambdas.
/// @see @ref ex09
class ui_work final {
private:
	HWND _hWnd;

public:
	explicit ui_work(HWND hWnd) : _hWnd{hWnd} { }

	/// Executes a function in the UI thread, allowing UI updates.
	///
	/// Internally, the function calls SendMessage() to switch to the UI thread.
	/// @tparam F `std::function<void()>`
	/// @param func `[]() {}`
	/// @return No value is returned. Note that this method is blocking: it returns
	/// only after the function is finished.
	///
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendmessagew
	template<typename F>
	auto work(F&& func) noexcept
		-> WINLAMB_LAMBDA_TYPE(func, void(), void)
	{
		std::function<void()>* pFunc = new std::function<void()>{std::move(func)};
		SendMessageW(this->_hWnd, _wli::WM_UI_WORK_THREAD,
			0xc0de'f00d, reinterpret_cast<LPARAM>(pFunc));
	}
};

}//namespace wl