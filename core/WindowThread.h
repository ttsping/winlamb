
#pragma once
#include <functional>
#include "Window.h"

namespace core {

class CustomControl;
class Dialog;

// A window with multithreading capabilities.
class WindowThread : public Window {
	friend CustomControl;
	friend Dialog;

public:
	virtual ~WindowThread() { }
	WindowThread() = default;

	// Runs code asynchronously in a new detached thread.
	void runDetachedThread(std::function<void()> func) const;

	// This method is analog to SendMessage (synchronous), but intended to be
	// called from another thread, so a callback function can, tunelled by
	// wndproc, run in the original thread of the window, thus allowing GUI
	// updates. This avoids the user to deal with a custom WM_ message.
	void runUiThread(std::function<void()> func) const;

private:
	[[nodiscard]] bool processUiThreadMsg(UINT msg, WPARAM wp, LPARAM lp) const;
};

}
