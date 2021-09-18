
#include <system_error>
#include "ImageList.h"
using namespace core;
using std::system_error;

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

void ImageList::loadIconResource(int iconIdx) const
{
	HICON hIcon = (HICON)LoadImageW(GetModuleHandle(nullptr), MAKEINTRESOURCEW(iconIdx),
		IMAGE_ICON, this->sz.cx, this->sz.cy, LR_DEFAULTCOLOR);
	if (!hIcon) {
		throw system_error(GetLastError(), std::system_category(), "LoadImageW failed");
	}

	if (ImageList_AddIcon(this->hil, hIcon) == -1) {
		throw system_error(GetLastError(), std::system_category(), "ImageList_AddIcon failed");
	}
	DestroyIcon(hIcon);
}
