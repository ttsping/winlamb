
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
	void create(const LOGFONT& lf);
	void getObject(LOGFONT& lf) const;
	[[nodiscard]] HFONT hFont() const noexcept { return this->hf; }

	static const Font& UiFont();
};

}
