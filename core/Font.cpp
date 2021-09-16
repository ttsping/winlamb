
#include <utility>
#include "Font.h"
#include <VersionHelpers.h>
using namespace core;

Font::Font(const LOGFONT& lf)
	: hf{CreateFontIndirectW(&lf)}
{
}

Font::Font(Font&& other) noexcept
	: hf{other.hf}
{
	other.hf = nullptr;
}

Font& Font::operator=(Font&& other) noexcept
{
	this->destroy();
	std::swap(this->hf, other.hf);
	return *this;
}

void Font::destroy() noexcept
{
	if (this->hf) {
		DeleteObject(this->hf);
		this->hf = nullptr;
	}
}

void Font::getObject(LOGFONT& lf) const
{
	GetObjectW(this->hf, sizeof(LOGFONT), &lf);
}

const Font& Font::UiFont()
{
	static Font globalUi;

	if (!globalUi.hFont()) { // not created yet?
		NONCLIENTMETRICS ncm = {0};
		ncm.cbSize = sizeof(ncm);
		if (!IsWindowsVistaOrGreater()) ncm.cbSize -= sizeof(int);
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
		globalUi = Font{ncm.lfMenuFont};
	}
	
	return globalUi;
}
