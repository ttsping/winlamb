
#pragma once
#include <Windows.h>

namespace core {

// Encapsulates an HFONT handle.
class Font final {
private:
	HFONT hf = nullptr;

public:
	~Font() { this->destroy(); }

	Font() = default;
	Font(const LOGFONT& lf);
	Font(Font&& other) noexcept;
	Font& operator=(Font&& other) noexcept;

	void destroy() noexcept;
	void getObject(LOGFONT& lf) const;
	HFONT hFont() const noexcept { return this->hf; }

	static const Font& UiFont();
};

}
