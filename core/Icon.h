
#pragma once
#include <optional>
#include <utility>
#include <string_view>
#include <Windows.h>

namespace core {

// Manages an HICON handle.
class Icon final {
private:
	HICON hIco = nullptr;

public:
	~Icon() { this->destroy(); }

	Icon() = default;
	explicit constexpr Icon(HICON hIco) : hIco{hIco} { }
	Icon(Icon&& other) noexcept { this->operator=(std::move(other)); }
	Icon& operator=(Icon&& other) noexcept;
	Icon& operator=(HICON hIco) noexcept { this->hIco = hIco; }

	void destroy() noexcept;
	[[nodiscard]] constexpr HICON hIcon() const { return this->hIco; }
	HICON leak();
	void loadResource(int iconId, SIZE resolution, std::optional<HINSTANCE> hInst = std::nullopt);
	void loadShell(std::wstring_view fileExtension, SIZE resolution);
};

}
