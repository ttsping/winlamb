/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <functional>
#include <optional>
#include <process.h>
#include <Windows.h>
#include "depot.h"
#include "lambda_type.h"
#include "msg_wm.h"
#include "msg_wnd_events_all.h"
#include "run.h"
#include "ui_work.h"

namespace _wli {

// Keeps the message handlers.
// Handles the UI thread message.
class base_msg_handler final {
private:
	depot<UINT> _depotWm;
	depot<UINT_PTR> _depotTimer;
	depot<WORD, WORD> _depotCmd;
	depot<WORD, int> _depotNfy;
	wl::msg::wnd_events_all _wndEventsAll;
	bool _isDlg;

public:
	base_msg_handler(bool isDlg)
		: _wndEventsAll{_depotWm, _depotTimer, _depotCmd, _depotNfy, isDlg}, _isDlg{isDlg}
	{
		this->_default_msg_handlers();
	}

	[[nodiscard]] wl::msg::wnd_events_all& on() noexcept { return this->_wndEventsAll; }

	// Searches for a stored handler for the given message and executes it, if any.
	std::optional<LRESULT> exec(UINT msg, WPARAM wp, LPARAM lp) noexcept
	{
		std::function<LRESULT(wl::msg::wm)>* userFunc = nullptr;

		if (msg == WM_TIMER) {
			userFunc = this->_depotTimer.find(static_cast<UINT_PTR>(wp));
		} else if (msg == WM_COMMAND) {
			userFunc = this->_depotCmd.find(LOWORD(wp), HIWORD(wp));
		} else if (msg == WM_NOTIFY) {
			const NMHDR* nmhdr = reinterpret_cast<const NMHDR*>(lp);
			userFunc = this->_depotNfy.find(static_cast<WORD>(nmhdr->idFrom), static_cast<int>(nmhdr->code));
		} else {
			userFunc = this->_depotWm.find(msg);
		}

		if (userFunc != nullptr) {
			LRESULT retVal = this->_isDlg ? TRUE : 0;
			run::catch_all_exceptions([&retVal, userFunc, wp, lp]() {
				retVal = (*userFunc)({wp, lp}); // execute user lambda
			}, run::post_quit_on_catch::YES);
			return {retVal};
		}
		return std::nullopt; // message handler not found
	}

	// Executes a function asynchronously, in a new detached background thread.
	// func: [](ui_work ui) {}
	template<typename F>
	auto background_work(HWND hWnd, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, void(wl::ui_work), void)
	{
		struct pack final {
			HWND hWnd;
			std::function<void(wl::ui_work)> func;
		};

		pack* pPack = new pack{hWnd, std::move(func)};

		HANDLE hThread = reinterpret_cast<HANDLE>(
			_beginthreadex(nullptr, 0,
				[](void* ptr) noexcept -> unsigned int {
					pack* pPack = reinterpret_cast<pack*>(ptr);
					run::catch_all_exceptions([pPack]() {
						pPack->func(wl::ui_work{pPack->hWnd}); // invoke user func
					}, run::post_quit_on_catch::NO);
					delete pPack;
					_endthreadex(0); // http://www.codeproject.com/Articles/7732/A-class-to-synchronise-thread-completions/
					return 0;
				},
				pPack, 0, nullptr)
		);

		if (hThread != nullptr) {
			CloseHandle(hThread);
		}
	}

private:
	void _default_msg_handlers()
	{
		this->on().wm(WM_UI_WORK_THREAD, [](wl::msg::wm p) noexcept -> LRESULT
		{
			// Handles our special message, sent by ui_work::work().
			if (p.wparam == 0xc0de'f00d && p.lparam != 0) {
				std::function<void()>* pFunc = reinterpret_cast<std::function<void()>*>(p.lparam); // retrieve back
				run::catch_all_exceptions([pFunc]() {
					(*pFunc)(); // invoke user func
				}, run::post_quit_on_catch::YES);
				delete pFunc;
			}
			return 0;
		});
	}
};

}//namespace _wli