
#include "ModalDialog.h"
using namespace core;

INT_PTR ModalDialog::show(Window* parent)
{
	return DialogBoxParamW((HINSTANCE)GetWindowLongPtrW(parent->hWnd(), GWLP_HINSTANCE),
		MAKEINTRESOURCEW(this->dialogId), parent->hWnd(),
		core_internals::Dialog::Proc, (LPARAM)this); // pass pointer to us
}
