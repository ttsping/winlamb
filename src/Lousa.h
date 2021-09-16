
#include "../core/CustomControl.h"

class Lousa : public core::CustomControl {
public:
	Lousa();

	virtual LRESULT windowProc(UINT msg, WPARAM wp, LPARAM lp) override;

private:
	void onCreate();
	void onClick(LPARAM lp);
};
