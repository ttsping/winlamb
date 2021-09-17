
#include <system_error>
#include "ListView.h"
#include <CommCtrl.h>
using namespace core;
using std::initializer_list;
using std::wstring_view;

UINT ListView::Columns::count() const
{
	HWND hHeader = (HWND)SendMessageW(this->lv.hWnd(), LVM_GETHEADER, 0, 0);
	if (!hHeader) {
		throw std::system_error(GetLastError(), std::system_category(), "LVM_GETHEADER failed");
	}

	int count = (int)SendMessageW(hHeader, HDM_GETITEMCOUNT, 0, 0);
	if (count == -1) {
		throw std::system_error(GetLastError(), std::system_category(), "HDM_GETITEMCOUNT failed");
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
		throw std::system_error(GetLastError(), std::system_category(), "LVM_INSERTCOLUMN failed");
	}

	return *this;
}

void ListView::Columns::stretch(int index) const
{
	UINT numCols = this->count();
	UINT cxUsed = 0;

	for (UINT i = 0; i < numCols; ++i) {
		if (i != index) {
			cxUsed += this->width(i); // retrieve cx of each column, but us
		}
	}

	RECT rc = {0};
	GetClientRect(this->lv.hWnd(), &rc); // ListView client area
	if (!SendMessageW(this->lv.hWnd(), LVM_SETCOLUMNWIDTH, index, rc.right - cxUsed)) {
		throw std::system_error(GetLastError(), std::system_category(), "LVM_SETCOLUMNWIDTH failed");
	}
}

UINT ListView::Columns::width(int index) const
{
	return (UINT)SendMessageW(this->lv.hWnd(), LVM_GETCOLUMNWIDTH, index, 0);
}

UINT ListView::Items::count() const
{
	return (int)SendMessageW(this->lv.hWnd(), LVM_GETITEMCOUNT, 0, 0);
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
		throw std::system_error(GetLastError(), std::system_category(), "LVM_INSERTITEM failed");
	}

	for (UINT i = 1; i < texts.size(); ++i) {
		lvi.iSubItem = i;
		lvi.pszText = (LPWSTR)(texts.begin() + i)->data();

		if (!SendMessageW(this->lv.hWnd(), LVM_SETITEMTEXT, newIdx, (LPARAM)&lvi)) {
			throw std::system_error(GetLastError(), std::system_category(), "LVM_SETITEMTEXT failed");
		}
	}

	return newIdx;
}

void ListView::Items::remove(int index) const
{
	if (!SendMessageW(this->lv.hWnd(), LVM_DELETEITEM, index, 0)) {
		throw std::system_error(GetLastError(), std::system_category(), "LVM_DELETEITEM failed");
	}
}

void ListView::setExtendedStyle(bool set, DWORD exStyles) const
{
	SendMessageW(this->hWnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, exStyles, set ? exStyles : 0);
}

void ListView::setImageList(const ImageList& imgLst, DWORD normalOrSmall) const
{
	SendMessageW(this->hWnd(), LVM_SETIMAGELIST, normalOrSmall, (LPARAM)imgLst.hImageList());
}
