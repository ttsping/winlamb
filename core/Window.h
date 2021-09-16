
#pragma once
#include <Windows.h>

namespace core {

class CustomControl;
class Dialog;

// Keeps the HWND handle. Base to all window classes.
class Window {
	friend CustomControl;
	friend Dialog;

private:
	HWND hw = nullptr;

public:
	virtual ~Window() { }

	HWND hWnd() const noexcept { return this->hw; }
};

}
