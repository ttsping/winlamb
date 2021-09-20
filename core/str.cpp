
#include <cwctype>
#include "str.h"
using namespace core;
using std::optional;
using std::wstring;
using std::wstring_view;

[[nodiscard]] static bool _EndsStartsFirstCheck(wstring_view s, wstring_view ending)
{
	if (s.empty()) return false;
	if (!ending.length() || ending.length() > s.length()) return false;
	return true;
}

static wstring _Format(wstring_view format, va_list args)
{
	size_t len = vswprintf(nullptr, 0, format.data(), args);
	wstring ret(len + 1, L'\0'); // room for terminating null
	vswprintf(&ret[0], len + 1, format.data(), args);
	ret.resize(len); // remove terminating null
	return ret;
}

void str::Dbg(wstring_view format, ...)
{
	va_list args;
	va_start(args, format);

	wstring s = _Format(format, args);

	va_end(args);
	OutputDebugStringW(s.c_str());
}

bool str::EndsWith(std::wstring_view s, std::wstring_view ending)
{
	if (!_EndsStartsFirstCheck(s, ending)) return false;
	return !wcsncmp(s.data() + s.length() - ending.length(),
		ending.data(), ending.length());
}

bool str::EndsWithI(std::wstring_view s, std::wstring_view ending)
{
	if (!_EndsStartsFirstCheck(s, ending)) return false;
	return !_wcsnicmp(s.data() + s.length() - ending.length(),
		ending.data(), ending.length());
}

bool str::EqI(std::wstring_view s1, std::wstring_view s2)
{
	return !lstrcmpiW(s1.data(), s2.data());
}

optional<UINT64> str::FindSubstr(wstring_view haystack, wstring_view needle, UINT64 offset)
{
	size_t index = haystack.find(needle, offset);
	return index == wstring::npos ? std::nullopt : optional{index};
}

optional<UINT64> str::FindSubstrI(wstring_view haystack, wstring_view needle, UINT64 offset)
{
	wstring haystack2 = str::ToUpper(haystack);
	wstring needle2 = str::ToUpper(needle);
	size_t index = haystack2.find(needle2, offset);
	return index == wstring::npos ? std::nullopt : optional{index};
}

optional<UINT64> str::FindSubstrRev(wstring_view haystack, wstring_view needle, UINT64 offset)
{
	size_t index = haystack.rfind(needle, offset);
	return index == wstring::npos ? std::nullopt : optional{index};
}

optional<UINT64> str::FindSubstrRevI(wstring_view haystack, wstring_view needle, UINT64 offset)
{
	wstring haystack2 = str::ToUpper(haystack);
	wstring needle2 = str::ToUpper(needle);
	size_t index = haystack2.rfind(needle2, offset);
	return index == wstring::npos ? std::nullopt : optional{index};
}

wstring str::Format(wstring_view format, ...)
{
	va_list args;
	va_start(args, format);

	wstring ret = _Format(format, args);

	va_end(args);
	return ret;
}

wstring& str::RemoveDiacritics(wstring& s)
{
	const wchar_t* diacritics   = L"¡·¿‡√„¬‚ƒ‰…È»Ë ÍÀÎÕÌÃÏŒÓœÔ”Û“Ú’ı‘Ù÷ˆ⁄˙Ÿ˘€˚‹¸«Á≈Â–—Òÿ¯›˝";
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

wstring& str::Replace(wstring& haystack, wstring_view needle, wstring_view replacement)
{
	if (haystack.empty() || needle.empty()) return haystack;

	wstring output(haystack.length(), L'\0'); // prealloc
	size_t base = 0;
	size_t found = 0;

	for (;;) {
		found = haystack.find(needle, found);
		output.insert(output.length(), haystack, base, found - base);
		if (found != wstring::npos) {
			output.append(replacement);
			base = found = found + needle.length();
		} else {
			break;
		}
	}

	haystack.swap(output); // behaves like an in-place operation
	return haystack;
}

wstring& str::ReplaceI(wstring& haystack, wstring_view needle, wstring_view replacement)
{
	if (haystack.empty() || needle.empty()) return haystack;

	wstring haystackU = str::ToUpper(haystack);
	wstring needleU = str::ToUpper(needle);

	wstring output(haystack.length(), L'\0'); // prealloc
	size_t base = 0;
	size_t found = 0;

	for (;;) {
		found = haystackU.find(needleU, found);
		output.insert(output.length(), haystack, base, found - base);
		if (found != std::wstring::npos) {
			output.append(replacement);
			base = found = found + needle.length();
		} else {
			break;
		}
	}

	haystack.swap(output); // behaves like an in-place operation
	return haystack;
}

wstring& str::Reverse(wstring& s)
{
	size_t lim = (s.length() - (s.length() % 2)) / 2;
	for (size_t i = 0; i < lim; ++i) {
		std::swap(s[i], s[s.length() - i - 1]);
	}
	return s;
}

bool str::StartsWith(wstring_view s, wstring_view start)
{
	if (!_EndsStartsFirstCheck(s, start)) return false;
	return !wcsncmp(s.data(), start.data(), start.length());
}

bool str::StartsWithI(wstring_view s, wstring_view start)
{
	if (!_EndsStartsFirstCheck(s, start)) return false;
	return !_wcsnicmp(s.data(), start.data(), start.length());
}

wstring str::ToLower(wstring_view s)
{
	wstring ret = s.data();
	CharLowerBuffW(&ret[0], static_cast<DWORD>(ret.length()));
	return ret;
}

wstring str::ToUpper(wstring_view s)
{
	wstring ret = s.data();
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
	if (!s.empty()) s.resize(lstrlenW(s.c_str()));
	return s;
}
