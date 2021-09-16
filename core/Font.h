
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
	explicit Font(const LOGFONT& lf) { this->create(lf); }
	Font(Font&& other) noexcept;
	Font& operator=(Font&& other) noexcept;

	void destroy() noexcept;
	void create(const LOGFONT& lf);
	void getObject(LOGFONT& lf) const;
	HFONT hFont() const noexcept { return this->hf; }

	static const Font& UiFont();
};

}
