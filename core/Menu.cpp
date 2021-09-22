
#include "Menu.h"
#include <system_error>
using namespace core;
using std::optional;
using std::system_error;

Menu::Menu(int menuId, optional<HINSTANCE> hInst)
	: hm{nullptr}
{
	if (!(this->hm = LoadMenu(hInst ? *hInst : GetModuleHandle(nullptr),
		MAKEINTRESOURCE(menuId))))
	{
		throw system_error(GetLastError(), std::system_category(), "LoadMenu failed");
	}
}

void Menu::destroy()
{
	if (this->hm) {
		DestroyMenu(this->hm);
		this->hm = nullptr;
	}
}

void Menu::showAtPoint(POINT pos, HWND hParent, optional<HWND> hCoordsRelativeTo) const
{
	ClientToScreen(hCoordsRelativeTo ? *hCoordsRelativeTo : hParent, &pos); // now relative to screen
	SetForegroundWindow(hParent);
	TrackPopupMenu(this->hm, TPM_LEFTBUTTON, pos.x, pos.y, 0, hParent, nullptr);
	PostMessageW(hParent, WM_NULL, 0, 0); // necessary according to TrackMenuPopup docs
}

Menu Menu::subMenu(UINT pos) const
{
	return Menu{GetSubMenu(this->hm, pos)};
}
