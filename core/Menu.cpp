
#include "Menu.h"
#include <system_error>
using namespace core;
using std::optional;
using std::system_error;

Menu& Menu::operator=(Menu&& other) noexcept
{
	this->destroy();
	std::swap(this->hm, other.hm);
	return *this;
}

void Menu::destroy() noexcept
{
	if (this->hm) {
		DestroyMenu(this->hm);
		this->hm = nullptr;
	}
}

HMENU Menu::leak()
{
	HMENU h = this->hm;
	this->hm = nullptr;
	return h;
}

HMENU Menu::subMenu(UINT pos) const
{
	return GetSubMenu(this->hm, pos);
}

void Menu::loadResource(int menuId, std::optional<HINSTANCE> hInst)
{
	this->destroy();

	if (!(this->hm = LoadMenuW(hInst ? *hInst : GetModuleHandleW(nullptr),
		MAKEINTRESOURCEW(menuId))))
	{
		throw system_error(GetLastError(), std::system_category(), "LoadMenuW failed");
	}
}

void Menu::showAtPoint(POINT pos, HWND hParent, optional<HWND> hCoordsRelativeTo) const
{
	ClientToScreen(hCoordsRelativeTo ? *hCoordsRelativeTo : hParent, &pos); // now relative to screen
	SetForegroundWindow(hParent);
	TrackPopupMenu(this->hm, TPM_LEFTBUTTON, pos.x, pos.y, 0, hParent, nullptr);
	PostMessageW(hParent, WM_NULL, 0, 0); // necessary according to TrackMenuPopup docs
}
