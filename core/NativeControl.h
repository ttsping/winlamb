
#pragma once
#include "Window.h"

namespace core {

// Base to all native controls.
class NativeControl : public Window {
public:
	virtual ~NativeControl() { }

	constexpr NativeControl(const NativeControl& other) noexcept : Window{other} { }
	constexpr NativeControl& operator=(const NativeControl& other) noexcept { this->Window::operator=(other); return *this; }

	explicit constexpr NativeControl(HWND hCtrl) noexcept : Window{hCtrl} { }
	NativeControl(HWND hParent, int ctrlId);
};

}
