
#include <system_error>
#include "Window.h"
#include "str.h"
using namespace core;
using std::system_error;
using std::wstring;
using std::wstring_view;

void Window::setText(wstring_view text) const
{
	SetWindowText(this->hw, text.data());
}

wstring Window::text() const
{
	wstring buf(GetWindowTextLength(this->hw) + 1, L'\0');
	if (!GetWindowText(this->hw, &buf[0], (int)buf.size())) {
		if (DWORD err = GetLastError(); err != ERROR_SUCCESS) {
			throw system_error(GetLastError(), std::system_category(), "GetWindowText failed");
		}
	}

	str::TrimNulls(buf);
	return buf;
}
