
#pragma once
#include <utility>
#include <Windows.h>

namespace core {

// Owning wrapper to HFONT handle.
class Font final {
private:
	HFONT hf;

public:
	~Font() { this->destroy(); }
	
	Font(Font&& other) noexcept { this->operator=(std::move(other)); }
	explicit constexpr Font(HFONT hf) noexcept : hf{hf} { }
	Font& operator=(Font&& other) noexcept;
	Font& operator=(HFONT hf) noexcept;

	explicit Font(const LOGFONT& lf);

	void destroy() noexcept;
	[[nodiscard]] constexpr HFONT hFont() const { return this->hf; }
	void getObject(LOGFONT& lf) const;

	static const Font& UiFont();
};

}
