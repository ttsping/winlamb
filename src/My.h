
#include "../core/MainDialog.h"

class My final : public core::MainDialog {
public:
	My();
	
protected:
	virtual INT_PTR dialogProc(UINT msg, WPARAM wp, LPARAM lp) override;

private:
	void onInitDialog();
	void onOk();
};
