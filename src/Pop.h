
#include "../core/ModalDialog.h"

class Pop final : core::ModalDialog::Handler {
private:
	core::ModalDialog wnd;

public:
	Pop();
	virtual INT_PTR dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) override;
	virtual INT_PTR show(HWND hParent) override { return this->wnd.show(hParent); }
};
