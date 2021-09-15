
#pragma once
#include <Windows.h>
#include "internals/Dialog.h"

namespace core {

// Creates and manages a modal window, created from a dialog resource.
class ModalDialog final {
public:
	// Implement this interface to handle messages from a ModalDialog object.
	class Handler : core_internals::Dialog::Handler {
	public:
		virtual INT_PTR show(HWND hParent) = 0;
	};

private:
	Handler* handler;
	int dialogId;
	
public:
	ModalDialog(Handler* handler, int dialogId)
		: handler{handler}, dialogId{dialogId} { }

	INT_PTR show(HWND hParent);
};

}
