
#pragma once
#include <utility>
#include <Windows.h>

namespace core {

// Manages an HFONT handle.
class Font final {
private:
	HFONT hf = nullptr;

public:
	~Font() { this->destroy(); }

	Font() = default;
	explicit constexpr Font(HFONT hf) : hf{hf} { }
	explicit Font(const LOGFONT& lf) { this->create(lf); }
	Font(Font&& other) noexcept { this->operator=(std::move(other)); }
	Font& operator=(Font&& other) noexcept;
	Font& operator=(HFONT hf) noexcept { this->hf = hf; }

	void destroy() noexcept;
	[[nodiscard]] constexpr HFONT hFont() const { return this->hf; }
	HFONT leak();
	void create(const LOGFONT& lf);
	void getObject(LOGFONT& lf) const;

	static const Font& UiFont();
};

}
