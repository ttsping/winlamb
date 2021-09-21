
#pragma once
#include <utility>
#include <Windows.h>

namespace core {

// Encapsulates an HFONT handle.
class Font final {
private:
	HFONT hf = nullptr;

public:
	~Font() { this->destroy(); }

	Font() = default;
	explicit Font(const LOGFONT& lf) { this->create(lf); }
	Font(Font&& other) noexcept { this->operator=(std::move(other)); }
	Font& operator=(Font&& other) noexcept;

	void destroy() noexcept;
	[[nodiscard]] constexpr HFONT hFont() const { return this->hf; }
	[[nodiscard]] HFONT leak();
	void create(const LOGFONT& lf);
	void getObject(LOGFONT& lf) const;

	static const Font& UiFont();
};

}
