
#include "../core/ModalDialog.h"

class Pop final : public core::ModalDialog {
public:
	Pop();

protected:
	virtual INT_PTR dialogProc(UINT msg, WPARAM wp, LPARAM lp) override;

private:
	void onInitDialog();
};