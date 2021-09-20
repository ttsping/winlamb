
#pragma once
#include "Dialog.h"

namespace core {

// Creates and manages a modal window, created from a dialog resource.
class ModalDialog : public Dialog {
public:
	virtual ~ModalDialog() { }

	constexpr ModalDialog(int dialogId)
		: Dialog{dialogId} { }

	virtual INT_PTR show(Window* parent);
};

}
