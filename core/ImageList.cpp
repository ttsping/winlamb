
#include <system_error>
#include "ImageList.h"
#include "internals.h"
using namespace core;
using std::initializer_list;
using std::invalid_argument;
using std::system_error;
using std::wstring_view;

ImageList::ImageList(SIZE resolution, UINT initialSize, DWORD flags)
	: sz{resolution}
{
	if (!(this->hil = ImageList_Create(resolution.cx, resolution.cy, flags, initialSize, 1))) {
		throw system_error(GetLastError(), std::system_category(), "ImageList_Create failed");
	}
}

ImageList& ImageList::operator=(ImageList&& other) noexcept
{
	this->destroy();
	std::swap(this->hil, other.hil);
	return *this;
}

void ImageList::destroy() noexcept
{
	ImageList_Destroy(this->hil);
}

HIMAGELIST ImageList::leak()
{
	HIMAGELIST hImgLst = this->hil;
	this->hil = nullptr;
	return hImgLst;
}

size_t ImageList::count() const
{
	return ImageList_GetImageCount(this->hil);
}

SIZE ImageList::resolution() const
{
	int cx = 0, cy = 0;
	ImageList_GetIconSize(this->hil, &cx, &cy);
	return SIZE{cx, cy};
}

void ImageList::loadIconResource(initializer_list<int> iconsIdx) const
{
	for (int iconIdx : iconsIdx) {
		HICON hIcon = (HICON)LoadImageW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(iconIdx),
			IMAGE_ICON, this->sz.cx, this->sz.cy, LR_DEFAULTCOLOR);
		if (!hIcon) {
			throw system_error(GetLastError(), std::system_category(), "LoadImageW failed");
		}

		if (ImageList_AddIcon(this->hil, hIcon) == -1) {
			throw system_error(GetLastError(), std::system_category(), "ImageList_AddIcon failed");
		}
		DestroyIcon(hIcon);
	}
}

void ImageList::loadShellIcon(initializer_list<wstring_view> fileExtensions) const
{
	SIZE iconRes = this->resolution();

	if (iconRes.cx != 16
		&& iconRes.cx != 32
		&& iconRes.cx != 48
		&& iconRes.cx != 256) throw invalid_argument("Unsupported icon size.");

	int shil;
	switch (iconRes.cx) {
	case 16: shil = SHIL_SMALL; break;
	case 32: shil = SHIL_LARGE; break;
	case 48: shil = SHIL_EXTRALARGE; break;
	case 256: shil = SHIL_JUMBO;
	}

	HIMAGELIST hilShell = core_internals::ShellImageList(shil); // http://stackoverflow.com/a/30496252

	for (wstring_view fileExtension : fileExtensions) {
		wchar_t extens[16] = {0};
		lstrcpyW(extens, (fileExtension[0] == L'.') ? L"*" : L"*."); // prepend dot if it doesn't have
		lstrcatW(extens, fileExtension.data());

		SHFILEINFO shfi = {0};
		if (!SHGetFileInfoW(extens, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi),
			SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX))
		{
			throw system_error(GetLastError(), std::system_category(), "SHGetFileInfoW failed.");
		}

		HICON hIcon = ImageList_GetIcon(hilShell, shfi.iIcon, ILD_NORMAL);
		if (!hIcon) {
			throw system_error(GetLastError(), std::system_category(), "ImageList_GetIcon failed.");
		}

		if (ImageList_AddIcon(this->hil, hIcon) == -1) {
			throw system_error(GetLastError(), std::system_category(), "ImageList_AddIcon failed.");
		}

		DestroyIcon(hIcon);
	}
}
