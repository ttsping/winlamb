
#include <system_error>
#include <utility>
#include "Font.h"
#include <VersionHelpers.h>
using namespace core;
using std::system_error;

Font& Font::operator=(Font&& other)
{
	this->destroy();
	std::swap(this->hf, other.hf);
	return *this;
}

Font& Font::operator=(HFONT hf)
{
	this->destroy();
	this->hf = hf;
	return *this;
}

Font::Font(const LOGFONT& lf)
	: hf{nullptr}
{
	if (!(this->hf = CreateFontIndirect(&lf))) {
		throw system_error(GetLastError(), std::system_category(), "CreateFontIndirect failed");
	}
}

void Font::destroy()
{
	if (this->hf) {
		DeleteObject(this->hf);
		this->hf = nullptr;
	}
}

void Font::getObject(LOGFONT& lf) const
{
	if (!GetObject(this->hf, sizeof(LOGFONT), &lf)) {
		throw system_error(GetLastError(), std::system_category(), "GetObject failed");
	}
}

const Font& Font::UiFont()
{
	static Font globalUi{nullptr};

	if (!globalUi.hFont()) { // not created yet?
		NONCLIENTMETRICS ncm = {0};
		ncm.cbSize = sizeof(ncm);
		if (!IsWindowsVistaOrGreater()) ncm.cbSize -= sizeof(int);
		if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0)) {
			throw system_error(GetLastError(), std::system_category(), "SystemParametersInfo failed");
		}
		globalUi = Font{ncm.lfMenuFont};
	}

	return globalUi;
}
