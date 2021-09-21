
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
	HWND hw;

public:
	virtual ~Window() { }

	constexpr Window() noexcept : hw{nullptr} { }
	explicit constexpr Window(HWND hWnd) noexcept : hw{hWnd} { }

	[[nodiscard]] constexpr HWND hWnd() const noexcept { return this->hw; }
};

}
