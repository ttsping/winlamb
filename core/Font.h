
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

	constexpr Font(Font&& other) : hf{nullptr} { std::swap(this->hf, other.hf); }
	Font& operator=(Font&& other);

	explicit constexpr Font(HFONT hf) : hf{hf} { }
	Font& operator=(HFONT hf);

	explicit Font(const LOGFONT& lf);

	void destroy();
	void getObject(LOGFONT& lf) const;
	[[nodiscard]] constexpr HFONT hFont() const { return this->hf; }

	static const Font& UiFont();
};

}
