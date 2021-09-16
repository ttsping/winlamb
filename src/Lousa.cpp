
#include "../core/str.h"
#include "Lousa.h"
using std::wstring;

Lousa::Lousa()
	: CustomControl{CS_DBLCLKS, LoadCursorW(nullptr, IDC_CROSS),
		nullptr, WS_EX_CLIENTEDGE,
		WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS}
{
}

LRESULT Lousa::windowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_CREATE: onCreate(); return 0;
	case WM_LBUTTONDOWN: onClick(lp); return 0;
	}
	return DefWindowProcW(hWnd(), msg, wp, lp);
}

void Lousa::onCreate()
{
	
}

void Lousa::onClick(LPARAM lp)
{
	POINT pt = {LOWORD(lp), HIWORD(lp)};
	core::str::Dbg(L"Clicked %d %d\n", pt.x, pt.y);
	
	
}
