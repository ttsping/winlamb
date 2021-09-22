
#pragma once
#include <optional>
#include <utility>
#include <string_view>
#include <Windows.h>

namespace core {

// Non-owning wrapper to HICON handle.
class Icon final {
private:
	HICON hIco;

public:
	constexpr Icon(const Icon& other) noexcept : hIco{other.hIco} { }
	constexpr Icon& operator=(const Icon& other) noexcept { this->hIco = other.hIco; return *this; }

	explicit constexpr Icon(HICON hIco) noexcept : hIco{hIco} { }
	constexpr Icon& operator=(HICON hIco) noexcept { this->hIco = hIco; return *this; }

	Icon(int iconId, SIZE resolution, std::optional<HINSTANCE> hInst = std::nullopt);
	Icon(std::wstring_view fileExtension, SIZE resolution);

	void destroy() noexcept;
	[[nodiscard]] constexpr HICON hIcon() const noexcept { return this->hIco; }
};

}
