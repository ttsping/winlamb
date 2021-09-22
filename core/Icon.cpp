
#include <system_error>
#include "Icon.h"
#include <CommCtrl.h>
#include <commoncontrols.h> // IID_IImageList
using namespace core;
using std::invalid_argument;
using std::optional;
using std::system_error;
using std::wstring_view;

Icon::Icon(int iconId, SIZE resolution, optional<HINSTANCE> hInst)
	: hIco{nullptr}
{
	if (!(this->hIco = (HICON)LoadImage(hInst ? *hInst : GetModuleHandle(nullptr),
		MAKEINTRESOURCE(iconId), IMAGE_ICON, resolution.cx, resolution.cy, LR_DEFAULTCOLOR)))
	{
		throw system_error(GetLastError(), std::system_category(), "LoadImage failed");
	}
}

Icon::Icon(wstring_view fileExtension, SIZE resolution)
	: hIco{nullptr}
{
	if (resolution.cx != 16
		&& resolution.cx != 32
		&& resolution.cx != 48
		&& resolution.cx != 256) throw invalid_argument("Unsupported icon size");

	int shil;
	switch (resolution.cx) {
	case 16: shil = SHIL_SMALL; break;
	case 32: shil = SHIL_LARGE; break;
	case 48: shil = SHIL_EXTRALARGE; break;
	case 256: shil = SHIL_JUMBO;
	}

	HIMAGELIST hilShell = nullptr;
	if (HRESULT hr = SHGetImageList(shil, IID_IImageList, (void**)(&hilShell)); FAILED(hr)) {
		throw system_error(hr, std::system_category(), "SHGetImageList failed");
	}

	wchar_t extens[16] = {0};
	lstrcpy(extens, (fileExtension[0] == L'.') ? L"*" : L"*."); // prepend dot if it doesn't have
	lstrcat(extens, fileExtension.data());

	SHFILEINFO shfi = {0};
	if (!SHGetFileInfo(extens, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi),
		SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX))
	{
		throw system_error(GetLastError(), std::system_category(), "SHGetFileInfo failed");
	}

	if (!(this->hIco = ImageList_GetIcon(hilShell, shfi.iIcon, ILD_NORMAL))) {
		throw system_error(GetLastError(), std::system_category(), "ImageList_GetIcon failed");
	}
}

void Icon::destroy()
{
	if (this->hIco) {
		DestroyIcon(this->hIco);
		this->hIco = nullptr;
	}
}
