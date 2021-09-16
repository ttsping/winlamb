
#include "Lousa.h"

Lousa::Lousa()
	: CustomControl{CS_DBLCLKS, nullptr, nullptr, WS_EX_CLIENTEDGE,
		WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS}
{
}

LRESULT Lousa::windowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_CREATE: onCreate(); return 0;
	}
	return DefWindowProcW(hWnd(), msg, wp, lp);
}

void Lousa::onCreate()
{
	
}
