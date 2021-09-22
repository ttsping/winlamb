
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
	constexpr Menu(const Menu& other) : hm{other.hm} { }
	constexpr Menu& operator=(const Menu& other) { this->hm = other.hm; return *this; }

	explicit constexpr Menu(HMENU hm) : hm{hm} { }
	constexpr Menu& operator=(HMENU hIco) { this->hm = hIco; return *this; }

	explicit Menu(int menuId, std::optional<HINSTANCE> hInst = std::nullopt);

	void destroy();
	[[nodiscard]] constexpr HMENU hMenu() const { return this->hm; }
	void showAtPoint(POINT pos, HWND hParent, std::optional<HWND> hCoordsRelativeTo = std::nullopt) const;
	[[nodiscard]] Menu subMenu(UINT pos) const;
};

}
