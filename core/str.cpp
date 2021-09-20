
#include <cwctype>
#include <stdexcept>
#include "str.h"
using namespace core;
using std::invalid_argument;
using std::optional;
using std::span;
using std::vector;
using std::wstring;
using std::wstring_view;

[[nodiscard]] static wstring _Format(wstring_view format, va_list args)
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

[[nodiscard]] static bool _EndsStartsFirstCheck(wstring_view s, wstring_view ending)
{
	if (s.empty()) return false;
	if (!ending.length() || ending.length() > s.length()) return false;
	return true;
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

optional<size_t> str::FindSubstr(wstring_view haystack, wstring_view needle, size_t offset)
{
	size_t index = haystack.find(needle, offset);
	return index == wstring::npos ? std::nullopt : optional{index};
}

optional<size_t> str::FindSubstrI(wstring_view haystack, wstring_view needle, size_t offset)
{
	wstring haystack2 = str::ToUpper(haystack);
	wstring needle2 = str::ToUpper(needle);
	size_t index = haystack2.find(needle2, offset);
	return index == wstring::npos ? std::nullopt : optional{index};
}

optional<size_t> str::FindSubstrRev(wstring_view haystack, wstring_view needle, size_t offset)
{
	size_t index = haystack.rfind(needle, offset);
	return index == wstring::npos ? std::nullopt : optional{index};
}

optional<size_t> str::FindSubstrRevI(wstring_view haystack, wstring_view needle, size_t offset)
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

optional<const wchar_t*> str::GetLineBreak(wstring_view s)
{
	for (size_t i = 0; i < s.length() - 1; ++i) {
		if (s[i] == L'\r') {
			return s[i + 1] == L'\n' ? optional{L"\r\n"} : optional{L"\r"};
		} else if (s[i] == L'\n') {
			return s[i + 1] == L'\r' ? optional{L"\n\r"} : optional{L"\n"};
		}
	}
	return std::nullopt; // unknown
}

[[nodiscard]] static wstring _ParseAscii(span<const BYTE> src)
{
	if (src.empty()) return {};

	wstring ret(src.size(), L'\0');
	for (size_t i = 0; i < src.size(); ++i) {
		if (!src[i]) { // found terminating null, stop processing here
			ret.resize(i);
			return ret;
		}
		ret[i] = static_cast<wchar_t>(src[i]); // raw conversion
	}

	str::TrimNulls(ret);
	return ret; // data didn't have a terminating null
}

[[nodiscard]] static wstring _ParseCodePage(span<const BYTE> src, UINT codePage)
{
	if (src.empty()) return {};

	wstring ret;
	int neededLen = MultiByteToWideChar(codePage, 0, (LPCCH)src.data(), (int)src.size(), nullptr, 0);
	ret.resize(neededLen);

	MultiByteToWideChar(codePage, 0, (LPCCH)src.data(), (int)src.size(), &ret[0], neededLen);
	str::TrimNulls(ret);
	return ret;
}

wstring str::Parse(span<const BYTE> src)
{
	if (src.empty()) return {};

	str::EncodingInfo encInfo = str::GetEncoding(src);
	src = src.subspan(encInfo.bomSize); // skip BOM, if any

	switch (encInfo.encType) {
	case Encoding::UNKNOWN:
	case Encoding::ASCII:   return _ParseAscii(src);
	case Encoding::WIN1252: return _ParseCodePage(src, 1252);
	case Encoding::UTF8:    return _ParseCodePage(src, CP_UTF8);
	case Encoding::UTF16BE: throw invalid_argument("UTF-16 big endian: encoding not implemented.");
	case Encoding::UTF16LE: throw invalid_argument("UTF-16 little endian: encoding not implemented.");
	case Encoding::UTF32BE: throw invalid_argument("UTF-32 big endian: encoding not implemented.");
	case Encoding::UTF32LE: throw invalid_argument("UTF-32 little endian: encoding not implemented.");
	case Encoding::SCSU:    throw invalid_argument("Standard compression scheme for Unicode: encoding not implemented.");
	case Encoding::BOCU1:   throw invalid_argument("Binary ordered compression for Unicode: encoding not implemented.");
	default:                throw invalid_argument("Unknown encoding.");
	}
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

vector<BYTE> str::SerializeToUtf8(wstring_view s, bool writeBom)
{
	if (s.empty()) return {};

	BYTE utf8Bom[] = {0xef, 0xbb, 0xbf};
	size_t szBom = writeBom ? ARRAYSIZE(utf8Bom) : 0;

	size_t neededLen = WideCharToMultiByte(CP_UTF8, 0, s.data(),
		(int)s.length(), nullptr, 0, nullptr, 0);

	vector<BYTE> ret(neededLen + szBom, 0x00);
	if (writeBom) memcpy(&ret[0], utf8Bom, szBom);
	
	WideCharToMultiByte(CP_UTF8, 0, s.data(), (int)s.length(),
		(char*)&ret[0 + szBom], (int)neededLen, nullptr, nullptr);
	return ret;
}

vector<wstring> str::Split(wstring_view s, wstring_view delimiter, optional<size_t> maxPieces, bool keepBlanks)
{
	vector<wstring> ret;
	size_t beginIdx = 0, curIdx = 0;

	for (;;) {
		if (optional<size_t> found = str::FindSubstr(s, delimiter, beginIdx); found) {
			curIdx = *found;
		} else {
			curIdx = s.length();
		}

		if (maxPieces && ret.size() + 1 == *maxPieces) {
			curIdx = s.length();
			if (keepBlanks || curIdx - beginIdx) ret.emplace_back(s, beginIdx, curIdx - beginIdx);
			break;
		}

		if (keepBlanks || curIdx - beginIdx) ret.emplace_back(s, beginIdx, curIdx - beginIdx);

		curIdx += delimiter.length();
		beginIdx = curIdx;
		if (curIdx >= s.length()) break;
	}

	return ret;
}

vector<wstring> str::SplitLines(wstring_view s)
{
	if (optional<const wchar_t*> lineBreak = str::GetLineBreak(s); lineBreak) {
		return str::Split(s, *lineBreak, std::nullopt, true);
	}
	return {wstring{s}}; // a single line
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

str::EncodingInfo str::GetEncoding(span<const BYTE> src)
{
	auto match = [&](span<const BYTE> bom) -> bool {
		return (src.size() >= bom.size())
			&& !memcmp(src.data(), bom.data(), bom.size_bytes());
	};

	// https://en.wikipedia.org/wiki/Byte_order_mark

	BYTE utf8[] = {0xef, 0xbb, 0xbf};
	if (match(utf8)) return {Encoding::UTF8, ARRAYSIZE(utf8)};

	BYTE utf16be[] = {0xfe, 0xff};
	if (match(utf16be)) return {Encoding::UTF16BE, ARRAYSIZE(utf16be)};

	BYTE utf16le[] = {0xff, 0xfe};
	if (match(utf16le)) return {Encoding::UTF16LE, ARRAYSIZE(utf16le)};

	BYTE utf32be[] = {0x00, 0x00, 0xfe, 0xff};
	if (match(utf32be)) return {Encoding::UTF32BE, ARRAYSIZE(utf32be)};

	BYTE utf32le[] = {0xff, 0xfe, 0x00, 0x00};
	if (match(utf32le)) return {Encoding::UTF32LE, ARRAYSIZE(utf32le)};

	BYTE scsu[] = {0x0e, 0xfe, 0xff};
	if (match(scsu)) return {Encoding::SCSU, ARRAYSIZE(scsu)};

	BYTE bocu1[] = {0xfb, 0xee, 0x28};
	if (match(bocu1)) return {Encoding::BOCU1, ARRAYSIZE(bocu1)};

	// No BOM found, guess UTF-8 without BOM, or Windows-1252 (superset of ISO-8859-1).
	bool canBeWin1252 = false;
	for (size_t i = 0; i < src.size(); ++i) {
		if (src[i] > 0x7f) { // 127
			canBeWin1252 = true;
			if (i <= src.size() - 2 && (
				(src[i] == 0xc2 && (src[i + 1] >= 0xa1 && src[i + 1] <= 0xbf)) || // http://www.utf8-chartable.de
				(src[i] == 0xc3 && (src[i + 1] >= 0x80 && src[i + 1] <= 0xbf))))
			{
				return {Encoding::UTF8, 0}; // UTF-8 without BOM
			}
		}
	}
	return {(canBeWin1252 ? Encoding::WIN1252 : Encoding::ASCII), 0};
}
