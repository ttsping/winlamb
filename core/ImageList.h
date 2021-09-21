
#pragma once
#include <string_view>
#include <Windows.h>
#include <CommCtrl.h>
#include "Icon.h"

namespace core {

// Manages an HIMAGELIST handle.
class ImageList final {
private:
	HIMAGELIST hil;

public:
	~ImageList() { this->destroy(); }

	constexpr ImageList(HIMAGELIST hil) : hil{hil} { }
	ImageList(SIZE resolution, UINT initialSize = 1, DWORD flags = ILC_COLOR32);
	ImageList(ImageList&& other) noexcept { this->operator=(std::move(other)); }
	ImageList& operator=(ImageList&& other) noexcept;
	ImageList& operator=(HIMAGELIST hil) noexcept { this->hil = hil; }

	void destroy() noexcept;
	[[nodiscard]] constexpr HIMAGELIST hImageList() const { return this->hil; }
	HIMAGELIST leak();
	[[nodiscard]] size_t count() const;
	[[nodiscard]] SIZE resolution() const;
	void load(const Icon& ico) const;
	void loadIconResource(std::initializer_list<int> iconsIdx) const;
	void loadShellIcon(std::initializer_list<std::wstring_view> fileExtensions) const;
};

}
