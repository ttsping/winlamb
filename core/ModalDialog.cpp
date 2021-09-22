
#include "ModalDialog.h"
using namespace core;

INT_PTR ModalDialog::show(Window* parent)
{
	return DialogBoxParam((HINSTANCE)GetWindowLongPtr(parent->hWnd(), GWLP_HINSTANCE),
		MAKEINTRESOURCE(this->dialogId), parent->hWnd(),
		Dialog::Proc, (LPARAM)this); // pass obj pointer to proc
}
