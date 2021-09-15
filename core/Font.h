
#pragma once
#include <Windows.h>

namespace core {

// Encapsulates an HFONT handle.
class Font final {
private:
	HFONT hFont = nullptr;

public:
	~Font() { this->destroy(); }
	Font() = default;
	Font(const LOGFONT& lf);
	Font(Font&& other) noexcept;
	Font& operator=(Font&& other) noexcept;

	void destroy() noexcept;
	void getObject(LOGFONT& lf) const;
	HFONT handle() const noexcept { return this->hFont; }

	static const Font& UiFont();
};

}
