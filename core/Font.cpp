
#include <system_error>
#include <utility>
#include "Font.h"
#include <VersionHelpers.h>
using namespace core;
using std::system_error;

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

HFONT Font::leak()
{
	HFONT h = this->hf;
	this->hf = nullptr;
	return h;
}

void Font::create(const LOGFONT& lf)
{
	if (!(this->hf = CreateFontIndirectW(&lf))) {
		throw system_error(GetLastError(), std::system_category(), "CreateFontIndirectW failed");
	}
}

void Font::getObject(LOGFONT& lf) const
{
	if (!GetObjectW(this->hf, sizeof(LOGFONT), &lf)) {
		throw system_error(GetLastError(), std::system_category(), "GetObjectW failed");
	}
}

const Font& Font::UiFont()
{
	static Font globalUi;

	if (!globalUi.hFont()) { // not created yet?
		NONCLIENTMETRICS ncm = {0};
		ncm.cbSize = sizeof(ncm);
		if (!IsWindowsVistaOrGreater()) ncm.cbSize -= sizeof(int);
		if (!SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0)) {
			throw system_error(GetLastError(), std::system_category(), "SystemParametersInfoW failed");
		}
		globalUi = Font{ncm.lfMenuFont};
	}

	return globalUi;
}
