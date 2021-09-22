
#pragma once
#include <string>
#include <string_view>
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

	constexpr Window(const Window& other) : hw{other.hw} { }
	constexpr Window& operator=(const Window& other) { this->hw = other.hw; return *this; }

	constexpr Window() : hw{nullptr} { }
	explicit constexpr Window(HWND hWnd) : hw{hWnd} { }

	[[nodiscard]] constexpr HWND hWnd() const { return this->hw; }
	void setText(std::wstring_view text) const;
	[[nodiscard]] std::wstring text() const;
};

}
