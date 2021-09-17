
#include <system_error>
#include "ImageList.h"
using namespace core;

ImageList::ImageList(SIZE resolution, DWORD flags, UINT initialSize)
{
	if (!(this->hil = ImageList_Create(resolution.cx, resolution.cy, flags, initialSize, 1))) {
		throw std::system_error(GetLastError(), std::system_category(), "ImageList_Create failed");
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
