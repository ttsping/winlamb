
#include <utility>
#include "Font.h"
#include <VersionHelpers.h>
using namespace core;

Font::Font(const LOGFONT& lf)
	: hFont{CreateFontIndirectW(&lf)}
{
}

Font::Font(Font&& other) noexcept
	: hFont{other.hFont}
{
	other.hFont = nullptr;
}

Font& Font::operator=(Font&& other) noexcept
{
	this->destroy();
	std::swap(this->hFont, other.hFont);
	return *this;
}

void Font::destroy() noexcept
{
	if (this->hFont) {
		DeleteObject(this->hFont);
		this->hFont = nullptr;
	}
}

void Font::getObject(LOGFONT& lf) const
{
	GetObjectW(this->hFont, sizeof(LOGFONT), &lf);
}

const Font& Font::UiFont()
{
	static Font globalUi;

	if (!globalUi.handle()) { // not created yet?
		NONCLIENTMETRICS ncm = {0};
		ncm.cbSize = sizeof(ncm);
		if (!IsWindowsVistaOrGreater()) ncm.cbSize -= sizeof(int);
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
		globalUi = Font{ncm.lfMenuFont};
	}
	
	return globalUi;
}
