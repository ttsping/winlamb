
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
	constexpr Icon(const Icon& other) : hIco{other.hIco} { }
	constexpr Icon& operator=(const Icon& other) { this->hIco = other.hIco; return *this; }

	explicit constexpr Icon(HICON hIco) : hIco{hIco} { }
	constexpr Icon& operator=(HICON hIco) { this->hIco = hIco; return *this; }

	Icon(int iconId, SIZE resolution, std::optional<HINSTANCE> hInst = std::nullopt);
	Icon(std::wstring_view fileExtension, SIZE resolution);

	void destroy();
	[[nodiscard]] constexpr HICON hIcon() const { return this->hIco; }
};

}
