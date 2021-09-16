
#include <cwctype>
#include "str.h"
using namespace core;
using std::wstring;

void str::Dbg(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);

	size_t len = vswprintf(nullptr, 0, format, args);
	wstring ret(len + 1, L'\0'); // room for terminating null
	vswprintf(&ret[0], len + 1, format, args);
	ret.resize(len); // remove terminating null

	va_end(args);
	OutputDebugStringW(ret.c_str());
}

wstring str::Format(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);

	size_t len = vswprintf(nullptr, 0, format, args);
	wstring ret(len + 1, L'\0'); // room for terminating null
	vswprintf(&ret[0], len + 1, format, args);
	ret.resize(len); // remove terminating null

	va_end(args);
	return ret;
}

wstring str::FormatError(DWORD errCode)
{
	wchar_t* buf = nullptr;
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buf, 0, nullptr);
	wstring text = buf;
	LocalFree(buf);
	return text;
}

wstring& str::RemoveDiacritics(wstring& s)
{
	const wchar_t* diacritics   = L"��������������������������������������������������������";
	const wchar_t* replacements = L"AaAaAaAaAaEeEeEeEeIiIiIiIiOoOoOoOoOoUuUuUuUuCcAaDdNnOoYy";
	for (wchar_t& ch : s) {
		const wchar_t* pDiac = diacritics;
		const wchar_t* pRepl = replacements;
		while (*pDiac) {
			if (ch == *pDiac) ch = *pRepl; // in-place replacement
			++pDiac;
			++pRepl;
		}
	}
	return s;
}

wstring& str::Reverse(wstring& s)
{
	size_t lim = (s.length() - (s.length() % 2)) / 2;
	for (size_t i = 0; i < lim; ++i) {
		std::swap(s[i], s[s.length() - i - 1]);
	}
	return s;
}

wstring str::ToLower(const wstring& s)
{
	wstring ret = s;
	CharLowerBuffW(&ret[0], static_cast<DWORD>(ret.length()));
	return ret;
}

wstring str::ToUpper(const wstring& s)
{
	wstring ret = s;
	CharUpperBuffW(&ret[0], static_cast<DWORD>(ret.length()));
	return ret;
}

wstring& str::Trim(wstring& s)
{
	if (s.empty()) return s;
	str::TrimNulls(s);

	size_t len = s.length();
	size_t iFirst = 0, iLast = len - 1; // bounds of trimmed string
	bool onlySpaces = true; // our string has only spaces?

	for (size_t i = 0; i < len; ++i) {
		if (!std::iswspace(s[i])) {
			iFirst = i;
			onlySpaces = false;
			break;
		}
	}
	if (onlySpaces) {
		s.clear();
		return s;
	}

	for (size_t i = len; i-- > 0; ) {
		if (!std::iswspace(s[i])) {
			iLast = i;
			break;
		}
	}

	std::copy(s.begin() + iFirst, // move the non-space chars back
		s.begin() + iLast + 1, s.begin());
	s.resize(iLast - iFirst + 1); // trim container size
	return s;
}

wstring& str::TrimNulls(wstring& s)
{
	// When a wstring is initialized with any length, possibly to be used as a buffer,
	// the string length may not match the size() method, after the operation.
	// This function fixes this.
	if (!s.empty()) {
		s.resize(lstrlenW(s.c_str()));
	}
	return s;
}
