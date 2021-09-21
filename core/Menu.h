
#pragma once
#include <optional>
#include <utility>
#include <Windows.h>

namespace core {

// Manages an HMENU handle.
class Menu final {
private:
	HMENU hm = nullptr;

public:
	~Menu() { this->destroy(); }

	Menu() = default;
	explicit constexpr Menu(HMENU hm) : hm{hm} { }
	Menu(Menu&& other) noexcept { this->operator=(std::move(other)); }
	Menu& operator=(Menu&& other) noexcept;
	Menu& operator=(HMENU hm) noexcept { this->hm = hm; }

	void destroy() noexcept;
	[[nodiscard]] constexpr HMENU hMenu() const { return this->hm; }
	HMENU leak();
	[[nodiscard]] HMENU subMenu(UINT pos) const;
	void loadResource(int menuId, std::optional<HINSTANCE> hInst = std::nullopt);
	void showAtPoint(POINT pos, HWND hParent, std::optional<HWND> hCoordsRelativeTo = std::nullopt) const;
};

}
