
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

	Window() = default;
	explicit Window(HWND hWnd) : hw{hWnd} { }

	[[nodiscard]] constexpr HWND hWnd() { return this->hw; }
};

}
