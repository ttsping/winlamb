
#include "CustomControl.h"
using namespace core;

void CustomControl::create(HWND hParent, int x, int y, int cx, int cy)
{
	WNDCLASSEXW wcx = {0};
	wcx.cbSize = sizeof(WNDCLASSEXW);
	wcx.style = this->classStyles;
	wcx.hCursor = this->hCursor ? this->hCursor : LoadCursorW(nullptr, IDC_ARROW);
	wcx.hbrBackground = this->hBrushBg ? this->hBrushBg : (HBRUSH)(COLOR_WINDOW + 1);

}

LRESULT CALLBACK CustomControl::Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	Handler *pHandler;

	if (msg == WM_NCCREATE) {
		CREATESTRUCT* cs = (CREATESTRUCT*)lp;
		pHandler = (Handler*)cs->lpCreateParams;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pHandler);
	} else {
		pHandler = (Handler*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	}

	return pHandler ? pHandler->windowProc(hWnd, msg, wp, lp) : 0;
}
