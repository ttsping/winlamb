
#pragma once
#include "Window.h"

namespace core {

// Base to all native controls.
class NativeControl : public Window {
public:
	virtual ~NativeControl() { }

	constexpr NativeControl(const NativeControl& other) : Window{other} { }
	constexpr NativeControl& operator=(const NativeControl& other) { this->Window::operator=(other); return *this; }

	explicit constexpr NativeControl(HWND hCtrl) : Window{hCtrl} { }
	NativeControl(HWND hParent, int ctrlId);
};

}
