
#include "ModalDialog.h"
using namespace core;

INT_PTR ModalDialog::show(HWND hParent)
{
	return DialogBoxParamW((HINSTANCE)GetWindowLongPtrW(hParent, GWLP_HINSTANCE),
		MAKEINTRESOURCEW(this->dialogId), hParent,
		core_internals::Dialog::Proc, (LPARAM)this->handler); // pass handler obj to proc	
}
