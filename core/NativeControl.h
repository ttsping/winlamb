
#pragma once
#include "Window.h"

namespace core {

// Base to all native controls.
class NativeControl : public Window {
public:
	explicit NativeControl(HWND hCtrl) : Window{hCtrl} { }
};

}