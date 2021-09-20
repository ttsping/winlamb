
#include "WindowThread.h"
#include "internals.h"
using namespace core;
using std::exception_ptr;
using std::function;

struct CallbackPack {
	function<void()> func;
	exception_ptr curExcept = nullptr;
};

void WindowThread::runUiThread(function<void()> func) const
{
	CallbackPack* pPack = new CallbackPack{std::move(func)};
	SendMessageW(this->hWnd(), core_internals::WM_UI_THREAD, 0, (LPARAM)pPack);
}

void WindowThread::processUiThreadMsg(LPARAM lp) const
{
	CallbackPack* pPack = (CallbackPack*)lp;
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
}
