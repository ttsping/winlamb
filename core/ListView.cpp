
#include <system_error>
#include "ListView.h"
#include "Menu.h"
#include <CommCtrl.h>
using namespace core;
using std::initializer_list;
using std::optional;
using std::system_error;
using std::wstring_view;

const ListView::Columns& ListView::Columns::add(wstring_view text, int size) const
{
	LVCOLUMN lvc = {0};
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = size;
	lvc.pszText = (LPWSTR)text.data();

	if (ListView_InsertColumn(this->lv.hWnd(), 0xffff, &lvc) == -1) {
		throw system_error(GetLastError(), std::system_category(), "ListView_InsertColumn failed");
	}

	return *this;
}

size_t ListView::Columns::count() const
{
	HWND hHeader = ListView_GetHeader(this->lv.hWnd());
	if (!hHeader) {
		throw system_error(GetLastError(), std::system_category(), "ListView_GetHeader failed");
	}

	size_t count = Header_GetItemCount(hHeader);
	if (count == -1) {
		throw system_error(GetLastError(), std::system_category(), "Header_GetItemCount failed");
	}

	return count;
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
	if (!ListView_SetColumnWidth(this->lv.hWnd(), index, (size_t)rc.right - cxUsed)) {
		throw system_error(GetLastError(), std::system_category(), "ListView_SetColumnWidth failed");
	}
}

size_t ListView::Columns::width(int index) const
{
	return ListView_GetColumnWidth(this->lv.hWnd(), index);
}

int ListView::Items::add(int iconIdx, initializer_list<wstring_view> texts) const
{
	LVITEM lvi = {0};
	lvi.mask = LVIF_TEXT | (iconIdx == -1 ? 0 : LVIF_IMAGE);
	lvi.iItem = 0x0fff'ffff; // insert as the last item
	lvi.iImage = iconIdx;
	lvi.pszText = (LPWSTR)texts.begin()->data();

	int newIdx = ListView_InsertItem(this->lv.hWnd(), &lvi);
	if (newIdx == -1) {
		throw system_error(GetLastError(), std::system_category(), "ListView_InsertItem failed");
	}

	for (size_t i = 1; i < texts.size(); ++i) {
		lvi.iSubItem = (int)i;
		lvi.pszText = (LPWSTR)(texts.begin() + i)->data();

		if (!SendMessage(this->lv.hWnd(), LVM_SETITEMTEXT, newIdx, (LPARAM)&lvi)) {
			throw system_error(GetLastError(), std::system_category(), "LVM_SETITEMTEXT failed");
		}
	}

	return newIdx;
}

size_t ListView::Items::count() const
{
	return ListView_GetItemCount(this->lv.hWnd());
}

optional<int> ListView::Items::focused() const
{
	int idx = ListView_GetNextItem(this->lv.hWnd(), -1, LVNI_FOCUSED);
	return idx == -1 ? std::nullopt : optional{idx};
}

bool ListView::Items::isSelected(int index) const
{
	return (ListView_GetItemState(this->lv.hWnd(), index, LVIS_SELECTED)
		& LVIS_SELECTED) != 0;
}

bool ListView::Items::isVisible(int index) const
{
	return ListView_IsItemVisible(this->lv.hWnd(), index) != 0;
}

LPARAM ListView::Items::lParam(int index) const
{
	LVITEM lvi = {0};
	lvi.iItem = index;
	lvi.mask = LVIF_PARAM;

	if (!ListView_GetItem(this->lv.hWnd(), &lvi)) {
		throw system_error(GetLastError(), std::system_category(), "ListView_GetItem failed");
	}

	return lvi.lParam;
}

RECT ListView::Items::rect(int index, int lvirPortion) const
{
	RECT rc = {0};
	if (!ListView_GetItemRect(this->lv.hWnd(), index, &rc, lvirPortion)) {
		throw system_error(GetLastError(), std::system_category(), "ListView_GetItemRect failed");
	}
	return rc;
}

void ListView::Items::remove(int index) const
{
	if (!ListView_DeleteItem(this->lv.hWnd(), index)) {
		throw system_error(GetLastError(), std::system_category(), "ListView_DeleteItem failed");
	}
}

void ListView::Items::selectAll(bool doSelect) const
{
	LVITEM lvi = {0};
	lvi.stateMask = LVIS_SELECTED;
	lvi.state = doSelect ? LVIS_SELECTED : 0;

	if (!SendMessage(this->lv.hWnd(), LVM_SETITEMSTATE, -1, (LPARAM)&lvi)) {
		throw system_error(GetLastError(), std::system_category(), "LVM_SETITEMSTATE failed.");
	}
}

void ListView::Items::setFocused(int index) const
{
	LVITEM lvi = {0};
	lvi.stateMask = LVIS_FOCUSED;
	lvi.state = LVIS_FOCUSED;

	if (!SendMessage(this->lv.hWnd(), LVM_SETITEMSTATE, index, (LPARAM)&lvi)) {
		throw system_error(GetLastError(), std::system_category(), "LVM_SETITEMSTATE failed.");
	}
}

void ListView::Items::setSelected(int index) const
{
	LVITEM lvi = {0};
	lvi.stateMask = LVIS_SELECTED;
	lvi.state = LVIS_SELECTED;

	if (!SendMessage(this->lv.hWnd(), LVM_SETITEMSTATE, index, (LPARAM)&lvi)) {
		throw system_error(GetLastError(), std::system_category(), "LVM_SETITEMSTATE failed.");
	}
}

void ListView::Items::setText(int itemIndex, int columnIndex, wstring_view text) const
{
	LVITEM lvi = {0};
	lvi.iSubItem = columnIndex;
	lvi.pszText = (LPWSTR)text.data();

	if (!SendMessage(this->lv.hWnd(), LVM_SETITEMTEXT, itemIndex, (LPARAM)&lvi)) {
		throw system_error(GetLastError(), std::system_category(), "LVM_SETITEMTEXT failed");
	}
}

ListView& ListView::operator=(const ListView& other)
{
	this->NativeControl::operator=(other);
	this->contextMenu = other.contextMenu;
	// Note that "columns" and "items" remain the same, pointing to "this".
	return *this;
}

int ListView::ctrlId() const
{
	return GetDlgCtrlID(this->hWnd());
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
	ListView_SetExtendedListViewStyleEx(this->hWnd(), exStyles, set ? exStyles : 0);
}

void ListView::setImageList(const ImageList& imgLst, DWORD normalOrSmall) const
{
	ListView_SetImageList(this->hWnd(), imgLst.hImageList(), normalOrSmall);
}

void ListView::setRedraw(bool doRedraw) const
{
	SendMessage(this->hWnd(), WM_SETREDRAW, (WPARAM)(BOOL)doRedraw, 0);
}

void ListView::setView(DWORD lvView) const
{
	if (ListView_SetView(this->hWnd(), lvView) == -1) {
		throw system_error(GetLastError(), std::system_category(), "ListView_SetView failed");
	}
}

DWORD ListView::view() const
{
	return ListView_GetView(this->hWnd());
}

void ListView::showContextMenu(bool followCursor, bool hasCtrl, bool hasShift) const
{
	if (!this->contextMenu) return;

	POINT menuPos = {0};

	if (followCursor) { // when fired by a right click
		GetCursorPos(&menuPos); // relative to screen
		ScreenToClient(this->hWnd(), &menuPos); // now relative to list view

		LVHITTESTINFO lvhti = {0};
		SendMessage(this->hWnd(), LVM_HITTEST, -1, (LPARAM)&lvhti);

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
