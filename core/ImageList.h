
#pragma once
#include <Windows.h>
#include <CommCtrl.h>

namespace core {

// Manages an image list handle.
class ImageList final {
private:
	HIMAGELIST hil;
	SIZE sz;

public:
	~ImageList() { this->destroy(); }

	ImageList(SIZE resolution, DWORD flags = ILC_COLOR32, UINT initialSize = 1);
	ImageList(ImageList&& other) noexcept { this->operator=(std::move(other)); }
	ImageList& operator=(ImageList&& other) noexcept;

	void destroy() noexcept;
	[[nodiscard]] constexpr HIMAGELIST hImageList() const { return this->hil; }
	void loadIconResource(int iconIdx) const;
};

}
