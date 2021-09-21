
#include <system_error>
#include "ImageList.h"
using namespace core;
using std::initializer_list;
using std::system_error;
using std::wstring_view;

ImageList::ImageList(SIZE resolution, UINT initialSize, DWORD flags)
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

void ImageList::load(const Icon& ico) const
{
	if (ImageList_AddIcon(this->hil, ico.hIcon()) == -1) {
		throw system_error(GetLastError(), std::system_category(), "ImageList_AddIcon failed");
	}
}

void ImageList::loadIconResource(initializer_list<int> iconsIdx) const
{
	SIZE icoRes = this->resolution();

	for (int iconIdx : iconsIdx) {
		Icon ico;
		ico.loadResource(iconIdx, icoRes);
		this->load(ico);
	}
}

void ImageList::loadShellIcon(initializer_list<wstring_view> fileExtensions) const
{
	SIZE icoRes = this->resolution();

	for (wstring_view fileExtension : fileExtensions) {
		Icon ico;
		ico.loadShell(fileExtension, icoRes);
		this->load(ico);
	}
}
