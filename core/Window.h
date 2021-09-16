
#pragma once
#include <Windows.h>

namespace core_internals {
class Dialog;


// Keeps the HWND handle. Base to all window classes.
class Window {
	friend Dialog;

private:
	HWND hw = nullptr;

public:
	HWND hWnd() const { return this->hw; }
};

}
