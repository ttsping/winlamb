
#include "ModalDialog.h"
using namespace core;

INT_PTR ModalDialog::show(Window* parent)
{
	return DialogBoxParamW((HINSTANCE)GetWindowLongPtrW(parent->hWnd(), GWLP_HINSTANCE),
		MAKEINTRESOURCEW(this->dialogId), parent->hWnd(),
		Dialog::Proc, (LPARAM)this); // pass obj pointer to proc
}
