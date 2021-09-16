
#include "CustomControl.h"
using namespace core;

void CustomControl::create(Window* parent, int x, int y, int cx, int cy)
{
	WNDCLASSEXW wcx = {0};
	wcx.cbSize = sizeof(WNDCLASSEXW);
	wcx.style = this->classStyles;
	wcx.hCursor = this->hCursor ? this->hCursor : LoadCursorW(nullptr, IDC_ARROW);
	wcx.hbrBackground = this->hBrushBg ? this->hBrushBg : (HBRUSH)(COLOR_WINDOW + 1);

}

LRESULT CALLBACK CustomControl::Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CustomControl* pObj;

	if (msg == WM_NCCREATE) {
		CREATESTRUCT* cs = (CREATESTRUCT*)lp;
		pObj = (CustomControl*)cs->lpCreateParams;
		pObj->hw = hWnd;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pObj);
	} else {
		pObj = (CustomControl*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	}

	return pObj ? pObj->windowProc(msg, wp, lp) : 0;
}
