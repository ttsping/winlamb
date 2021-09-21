
#pragma once
#include <string_view>
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

	ImageList(SIZE resolution, UINT initialSize = 1, DWORD flags = ILC_COLOR32);
	ImageList(ImageList&& other) noexcept { this->operator=(std::move(other)); }
	ImageList& operator=(ImageList&& other) noexcept;

	void destroy() noexcept;
	[[nodiscard]] constexpr HIMAGELIST hImageList() const { return this->hil; }
	[[nodiscard]] HIMAGELIST leak();
	[[nodiscard]] size_t count() const;
	[[nodiscard]] SIZE resolution() const;
	void loadIconResource(std::initializer_list<int> iconsIdx) const;
	void loadShellIcon(std::initializer_list<std::wstring_view> fileExtensions) const;
};

}
