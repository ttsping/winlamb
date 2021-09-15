
#include "../core/MainDialog.h"

class My final : core::MainDialog::Handler {
private:
	core::MainDialog wnd;

public:
	My();
	virtual INT_PTR dialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) override;
	virtual int run(HINSTANCE hInst, int cmdShow) override { return this->wnd.run(hInst, cmdShow); }

protected:
	void onInitDialog(HWND hDlg);
};
