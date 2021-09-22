
#include <system_error>
#include "NativeControl.h"
using namespace core;
using std::system_error;

NativeControl::NativeControl(HWND hParent, int ctrlId)
	: Window{GetDlgItem(hParent, ctrlId)}
{
	if (!this->hWnd()) {
		throw system_error(GetLastError(), std::system_category(), "GetDlgItem failed");
	}
}
