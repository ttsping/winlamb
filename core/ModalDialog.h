
#pragma once
#include <Windows.h>
#include "Dialog.h"

namespace core {

// Creates and manages a modal window, created from a dialog resource.
class ModalDialog : public core_internals::Dialog {
private:
	int dialogId;
	
public:
	ModalDialog(int dialogId)
		: dialogId{dialogId} { }

	virtual INT_PTR show(HWND hParent);
};

}
