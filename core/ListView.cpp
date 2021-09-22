
#include <system_error>
#include "ListView.h"
#include "Menu.h"
#include <CommCtrl.h>
using namespace core;
using std::initializer_list;
using std::optional;
using std::system_error;
using std::wstring_view;

size_t ListView::Columns::count() const
{
	HWND hHeader = (HWND)SendMessageW(this->lv.hWnd(), LVM_GETHEADER, 0, 0);
	if (!hHeader) {
		throw system_error(GetLastError(), std::system_category(), "LVM_GETHEADER failed");
	}

	size_t count = (size_t)SendMessageW(hHeader, HDM_GETITEMCOUNT, 0, 0);
	if (count == -1) {
		throw system_error(GetLastError(), std::system_category(), "HDM_GETITEMCOUNT failed");
	}

	return count;
}

const ListView::Columns& ListView::Columns::add(std::wstring_view text, int size) const
{
	LVCOLUMNW lvc = {0};
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = size;
	lvc.pszText = (LPWSTR)text.data();

	if (SendMessageW(this->lv.hWnd(), LVM_INSERTCOLUMN, 0xffff, (LPARAM)&lvc) == -1) {
		throw system_error(GetLastError(), std::system_category(), "LVM_INSERTCOLUMN failed");
	}

	return *this;
}

void ListView::Columns::stretch(int index) const
{
	int numCols = (int)this->count();
	size_t cxUsed = 0;

	for (int i = 0; i < numCols; ++i) {
		if (i != index) {
			cxUsed += this->width(i); // retrieve cx of each column, but us
		}
	}

	RECT rc = {0};
	GetClientRect(this->lv.hWnd(), &rc); // ListView client area
	if (!SendMessageW(this->lv.hWnd(), LVM_SETCOLUMNWIDTH, index, (size_t)rc.right - cxUsed)) {
		throw system_error(GetLastError(), std::system_category(), "LVM_SETCOLUMNWIDTH failed");
	}
}

size_t ListView::Columns::width(int index) const
{
	return (size_t)SendMessageW(this->lv.hWnd(), LVM_GETCOLUMNWIDTH, index, 0);
}

size_t ListView::Items::count() const
{
	return (size_t)SendMessageW(this->lv.hWnd(), LVM_GETITEMCOUNT, 0, 0);
}

UINT ListView::Items::add(int iconIdx, initializer_list<wstring_view> texts) const
{
	LVITEMW lvi = {0};
	lvi.mask = LVIF_TEXT | (iconIdx == -1 ? 0 : LVIF_IMAGE);
	lvi.iItem = 0x0fff'ffff; // insert as the last item
	lvi.iImage = iconIdx;
	lvi.pszText = (LPWSTR)texts.begin()->data();

	int newIdx = (int)SendMessageW(this->lv.hWnd(), LVM_INSERTITEM, 0, (LPARAM)&lvi);
	if (newIdx == -1) {
		throw system_error(GetLastError(), std::system_category(), "LVM_INSERTITEM failed");
	}

	for (size_t i = 1; i < texts.size(); ++i) {
		lvi.iSubItem = (int)i;
		lvi.pszText = (LPWSTR)(texts.begin() + i)->data();

		if (!SendMessageW(this->lv.hWnd(), LVM_SETITEMTEXT, newIdx, (LPARAM)&lvi)) {
			throw system_error(GetLastError(), std::system_category(), "LVM_SETITEMTEXT failed");
		}
	}

	return newIdx;
}

void ListView::Items::remove(int index) const
{
	if (!SendMessageW(this->lv.hWnd(), LVM_DELETEITEM, index, 0)) {
		throw system_error(GetLastError(), std::system_category(), "LVM_DELETEITEM failed");
	}
}

void ListView::Items::selectAll(bool doSelect) const
{
	LVITEMW lvi = {0};
	lvi.stateMask = LVIS_SELECTED;
	lvi.state = doSelect ? LVIS_SELECTED : 0;

	if (!SendMessageW(this->lv.hWnd(), LVM_SETITEMSTATE, -1, (LPARAM)&lvi)) {
		throw system_error(GetLastError(), std::system_category(), "LVM_SETITEMSTATE failed.");
	}
}

optional<int> ListView::Items::focused() const
{
	int idx = (int)SendMessageW(this->lv.hWnd(), LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
	return idx == -1 ? std::nullopt : optional{idx};
}

void ListView::Items::setFocused(int index) const
{
	LVITEMW lvi = {0};
	lvi.stateMask = LVIS_FOCUSED;
	lvi.state = LVIS_FOCUSED;

	if (SendMessageW(this->lv.hWnd(), LVM_SETITEMSTATE, index, (LPARAM)&lvi) == -1) {
		throw system_error(GetLastError(), std::system_category(), "LVM_SETITEMSTATE failed.");
	}
}

bool ListView::Items::isVisible(int index) const
{
	return SendMessageW(this->lv.hWnd(), LVM_ISITEMVISIBLE, index, 0) != 0;
}

RECT ListView::Items::rect(int index, int lvirPortion) const
{
	RECT rc = {0};
	rc.left = lvirPortion;

	if (!SendMessageW(this->lv.hWnd(), LVM_GETITEMRECT, index, (LPARAM)&rc)) {
		throw system_error(GetLastError(), std::system_category(), "LVM_GETITEMRECT failed");
	}
	return rc;
}

ListView& ListView::operator=(const ListView& other) noexcept
{
	this->NativeControl::operator=(other);
	this->contextMenu = other.contextMenu;
	// Note that "columns" and "items" remain the same, pointing to "this".
	return *this;
}

bool ListView::onWmNotify(LPARAM lp) const
{
	if (const NMHDR* pNm = (const NMHDR*)lp; pNm->idFrom == this->ctrlId()) {
		if (pNm->code == LVN_KEYDOWN) {
			const NMLVKEYDOWN* pNkd = (const NMLVKEYDOWN*)lp;
			bool hasCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool hasShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

			if (hasCtrl && pNkd->wVKey == 'A') { // Ctrl+A pressed?
				this->items.selectAll(true);
				return true;
			} else if (pNkd->wVKey == VK_APPS) { // context menu key
				this->showContextMenu(false, hasCtrl, hasShift);
				return true;
			}

		} else if (pNm->code == NM_RCLICK) {
			const NMITEMACTIVATE* pNia = (const NMITEMACTIVATE*)lp;
			bool hasCtrl = (pNia->uKeyFlags & LVKF_CONTROL) != 0;
			bool hasShift = (pNia->uKeyFlags & LVKF_SHIFT) != 0;

			this->showContextMenu(true, hasCtrl, hasShift);
			return true;
		}
	}

	return false;
}

void ListView::setExtendedStyle(bool set, DWORD exStyles) const
{
	SendMessageW(this->hWnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, exStyles, set ? exStyles : 0);
}

void ListView::setImageList(const ImageList& imgLst, DWORD normalOrSmall) const
{
	SendMessageW(this->hWnd(), LVM_SETIMAGELIST, normalOrSmall, (LPARAM)imgLst.hImageList());
}

void ListView::showContextMenu(bool followCursor, bool hasCtrl, bool hasShift) const
{
	if (!this->contextMenu) return;

	POINT menuPos = {0};

	if (followCursor) { // when fired by a right click
		GetCursorPos(&menuPos); // relative to screen
		ScreenToClient(this->hWnd(), &menuPos); // now relative to list view

		LVHITTESTINFO lvhti = {0};
		SendMessageW(this->hWnd(), LVM_HITTEST, -1, (LPARAM)&lvhti);

		if (lvhti.iItem == -1) { // no item was right-clicked
			this->items.selectAll(false);
		} else if (!hasCtrl && !hasShift) {
			this->items.setFocused(lvhti.iItem);
		}
		SetFocus(this->hWnd()); // because a right-click won't set the focus by itself

	} else { // usually fired with the context keyboard key
		if (optional<int> focusIdx = this->items.focused();
			focusIdx && this->items.isVisible(*focusIdx))
		{
			RECT rcItem = this->items.rect(*focusIdx);
			menuPos.x = rcItem.left + 16; // arbitrary
			menuPos.y = rcItem.top + (rcItem.bottom - rcItem.top) / 2;
		} else {
			menuPos.x = 6; // arbitrary anchor coords
			menuPos.y = 10;
		}
	}

	this->contextMenu->showAtPoint(menuPos, GetParent(this->hWnd()), optional{this->hWnd()});
}
