
#include <process.h>
#include "WindowThread.h"
#include "internals.h"
using namespace core;
using std::exception_ptr;
using std::function;

// Transports data between threads.
struct ThreadPack {
	function<void()> func;
	HWND hWnd = nullptr;
	exception_ptr curExcept = nullptr;
};

static const UINT WM_UI_THREAD = WM_APP + 0x3fff;

void WindowThread::runUiThread(function<void()> func) const
{
	ThreadPack* pPack = new ThreadPack{std::move(func)};
	SendMessageW(this->hWnd(), WM_UI_THREAD, 0, (LPARAM)pPack);
}

void WindowThread::runDetachedThread(function<void()> func) const
{
	// Analog to std::thread([](){ ... }).detach(), but exception-safe.

	ThreadPack* pPack = new ThreadPack{std::move(func), this->hWnd()};

	HANDLE hThread = (HANDLE)_beginthreadex(nullptr, 0, [](void* ptr) -> UINT {
		ThreadPack* pPack = (ThreadPack*)ptr;
		try {
			pPack->func();
		} catch (...) {
			ThreadPack* pPackCrash = new ThreadPack{[]{}, pPack->hWnd, std::current_exception()};
			SendMessageW(pPack->hWnd, WM_UI_THREAD, 0, (LPARAM)pPackCrash);
		}
		delete pPack;
		_endthreadex(0); // http://www.codeproject.com/Articles/7732/A-class-to-synchronise-thread-completions/
		return 0;
	}, pPack, 0, nullptr);

	if (hThread) CloseHandle(hThread);
}

bool WindowThread::processUiThreadMsg(UINT msg, LPARAM lp) const
{
	if (msg != WM_UI_THREAD) return false;

	ThreadPack* pPack = (ThreadPack*)lp;
	try {
		if (pPack->curExcept) {
			std::rethrow_exception(pPack->curExcept);
		} else {
			pPack->func();
		}
	} catch (...) {
		PostQuitMessage(core_internals::Lippincott());
	}
	delete pPack;

	return true;
}
