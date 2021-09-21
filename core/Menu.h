
#pragma once
#include <optional>
#include <utility>
#include <Windows.h>

namespace core {

// Non-owning wrapper to HMENU handle.
class Menu final {
private:
	HMENU hm;

public:
	constexpr Menu(const Menu& other) noexcept : hm{other.hm} { }
	explicit constexpr Menu(HMENU hm) noexcept : hm{hm} { }
	constexpr Menu& operator=(const Menu& other) noexcept { this->hm = other.hm; return *this; }
	constexpr Menu& operator=(HMENU hIco) noexcept { this->hm = hIco; return *this; }

	explicit Menu(int menuId, std::optional<HINSTANCE> hInst = std::nullopt);

	void destroy() noexcept;
	[[nodiscard]] constexpr HMENU hMenu() const noexcept { return this->hm; }
	[[nodiscard]] Menu subMenu(UINT pos) const noexcept;
	void showAtPoint(POINT pos, HWND hParent, std::optional<HWND> hCoordsRelativeTo = std::nullopt) const;
};

}
