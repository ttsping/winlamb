
#pragma once
#include <string_view>
#include <Windows.h>
#include <CommCtrl.h>
#include "Icon.h"

namespace core {

// Owning wrapper to HIMAGELIST handle.
class ImageList final {
private:
	HIMAGELIST hil;

public:
	~ImageList() { this->destroy(); }

	ImageList(ImageList&& other) noexcept { this->operator=(std::move(other)); }
	explicit constexpr ImageList(HIMAGELIST hil) noexcept : hil{hil} { }
	ImageList& operator=(ImageList&& other) noexcept;
	ImageList& operator=(HIMAGELIST hil) noexcept;

	explicit ImageList(SIZE resolution, UINT initialSize = 1, DWORD ilcFlags = ILC_COLOR32);
	
	void destroy() noexcept;
	[[nodiscard]] constexpr HIMAGELIST hImageList() const noexcept { return this->hil; }
	[[nodiscard]] size_t count() const noexcept;
	[[nodiscard]] SIZE resolution() const;
	void load(const Icon& ico) const;
	void loadIconResource(std::initializer_list<int> iconsIdx) const;
	void loadShellIcon(std::initializer_list<std::wstring_view> fileExtensions) const;
};

}
