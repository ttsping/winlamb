
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

	constexpr ImageList(ImageList&& other) : hil{nullptr} { std::swap(this->hil, other.hil); }
	ImageList& operator=(ImageList&& other);

	explicit constexpr ImageList(HIMAGELIST hil) : hil{hil} { }
	ImageList& operator=(HIMAGELIST hil);

	explicit ImageList(SIZE resolution, UINT initialSize = 1, DWORD ilcFlags = ILC_COLOR32);

	[[nodiscard]] size_t count() const;
	void destroy();
	[[nodiscard]] constexpr HIMAGELIST hImageList() const { return this->hil; }
	void load(const Icon& ico) const;
	void loadIconResource(std::initializer_list<int> iconsIdx) const;
	void loadShellIcon(std::initializer_list<std::wstring_view> fileExtensions) const;
	[[nodiscard]] SIZE resolution() const;
};

}
