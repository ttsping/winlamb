
#pragma once
#include <utility>
#include <string_view>
#include <Windows.h>

namespace core {

// Encapsulates an HICON handle.
class Icon final {
private:
	HICON hIco = nullptr;

public:
	~Icon() { this->destroy(); }

	Icon() = default;
	Icon(Icon&& other) noexcept { this->operator=(std::move(other)); }
	Icon& operator=(Icon&& other) noexcept;

	void destroy() noexcept;
	[[nodiscard]] constexpr HICON hIcon() const { return this->hIco; }
	[[nodiscard]] HICON leak();
	void loadResource(int iconId, SIZE resolution);
	void loadShell(std::wstring_view fileExtension, SIZE resolution);
};

}
