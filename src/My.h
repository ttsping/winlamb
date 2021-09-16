
#include "../core/MainDialog.h"
#include "Lousa.h"

class My final : public core::MainDialog {
private:
	Lousa lousa;

public:
	My();
	
protected:
	virtual INT_PTR dialogProc(UINT msg, WPARAM wp, LPARAM lp) override;

private:
	void onInitDialog();
	void onOk();
};
