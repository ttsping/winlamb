
#include <system_error>
#include "ImageList.h"
using namespace core;
using std::initializer_list;
using std::optional;
using std::system_error;
using std::wstring_view;

ImageList& ImageList::operator=(ImageList&& other)
{
	this->destroy();
	std::swap(this->hil, other.hil);
	return *this;
}

ImageList& ImageList::operator=(HIMAGELIST hil)
{
	this->destroy();
	this->hil = hil;
	return *this;
}

ImageList::ImageList(SIZE resolution, UINT initialSize, DWORD ilcFlags)
	: hil{nullptr}
{
	if (!(this->hil = ImageList_Create(resolution.cx, resolution.cy, ilcFlags, initialSize, 1))) {
		throw system_error(GetLastError(), std::system_category(), "ImageList_Create failed");
	}
}

size_t ImageList::count() const
{
	return ImageList_GetImageCount(this->hil);
}

void ImageList::destroy()
{
	if (this->hil) {
		ImageList_Destroy(this->hil);
		this->hil = nullptr;
	}
}

void ImageList::load(const Icon& ico) const
{
	if (ImageList_AddIcon(this->hil, ico.hIcon()) == -1) {
		throw system_error(GetLastError(), std::system_category(), "ImageList_AddIcon failed");
	}
}

void ImageList::loadIconResource(initializer_list<int> iconsIdx) const
{
	HINSTANCE hInst = GetModuleHandle(nullptr);
	SIZE icoRes = this->resolution();
	for (int iconIdx : iconsIdx) {
		this->load(Icon{iconIdx, icoRes, optional{hInst}});
	}
}

void ImageList::loadShellIcon(initializer_list<wstring_view> fileExtensions) const
{
	SIZE icoRes = this->resolution();
	for (const wstring_view& fileExtension : fileExtensions) {
		this->load(Icon{fileExtension, icoRes});
	}
}

SIZE ImageList::resolution() const
{
	int cx = 0, cy = 0;
	if (!ImageList_GetIconSize(this->hil, &cx, &cy)) {
		throw system_error(GetLastError(), std::system_category(), "ImageList_GetIconSize failed");
	}
	return SIZE{cx, cy};
}
