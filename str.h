/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <cwctype>
#include <optional>
#include <string_view>
#include <vector>
#include <Windows.h>
#include "internal/bin_aux.h"
#include "internal/str_aux.h"

/// String utilities.
///
/// #include <str.h>
namespace wl::str {

/// Type-safe sprintf, which also accepts wstring and wstring_view as argument.
/// @param strFormat Formatting string. Ex.: `L"Name %s and age %d, %.2f percent."`
/// @param args Arguments to the format placements.
/// @return New string.
/// @see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/sprintf-sprintf-l-swprintf-swprintf-l-swprintf-l
template<typename ...argsT>
[[nodiscard]] inline std::wstring format(std::wstring_view strFormat, const argsT&... args)
{
	return _wli::str_aux::format_raw(strFormat, std::forward<const argsT&>(args)...);
}

/// OutputDebugString() with type-safe sprintf, which also accepts wstring and wstring_view as argument.
/// @param strFormat Formatting string. Ex.: `L"Name %s and age %d, %.2f percent."`
/// @param args Arguments to the format placements.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-outputdebugstringw
/// @see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/sprintf-sprintf-l-swprintf-swprintf-l-swprintf-l
template<typename ...argsT>
[[nodiscard]] inline void debug(std::wstring_view strFormat, const argsT&... args)
{
#ifdef _DEBUG
	OutputDebugStringW(
		_wli::str_aux::format_raw(strFormat, std::forward<const argsT&>(args)...).c_str() );
#endif
}

/// Converts ANSI string to Unicode wstring.
/// @see https://docs.microsoft.com/en-us/windows/win32/learnwin32/working-with-strings
/// @return New string.
[[nodiscard]] inline std::wstring ansi_to_unicode(std::string_view s)
{
	return _wli::bin_aux::str_from_ansi(
		reinterpret_cast<const BYTE*>(s.data()), s.length());
}

/// Converts Unicode wstring to ANSI string.
/// @see https://docs.microsoft.com/en-us/windows/win32/learnwin32/working-with-strings
/// @return New string.
[[nodiscard]] inline std::string unicode_to_ansi(std::wstring_view s)
{
	std::string ret(s.length(), '\0');
	for (size_t i = 0; i < s.length(); ++i) {
		ret[i] = static_cast<char>(s[i]); // raw conversion
	}
	return ret;
}

/// Returns a new string converted to lowercase.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-charlowerbuffw
/// @return New string.
[[nodiscard]] inline std::wstring to_lower(std::wstring_view s)
{
	std::wstring buf = s.data();
	CharLowerBuffW(&buf[0], static_cast<DWORD>(buf.length()));
	return buf;
}

/// Returns a new string converted to uppercase.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-charupperbuffw
/// @return New string.
[[nodiscard]] inline std::wstring to_upper(std::wstring_view s)
{
	std::wstring buf = s.data();
	CharUpperBuffW(&buf[0], static_cast<DWORD>(buf.length()));
	return buf;
}

/// Checks if two strings are equal, case sensitive.
/// Same of wstring::operator==().
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lstrcmpw
[[nodiscard]] inline bool eq(std::wstring_view s, std::wstring_view what) noexcept
{
	return lstrcmpW(s.data(), what.data()) == 0;
}

/// Checks if two strings are equal, case insensitive.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lstrcmpiw
[[nodiscard]] inline bool eqi(std::wstring_view s, std::wstring_view what) noexcept
{
	return lstrcmpiW(s.data(), what.data()) == 0;
}

/// Checks, case sensitive, if the string begins with the given text.
/// @see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strncmp-wcsncmp-mbsncmp-mbsncmp-l
[[nodiscard]] inline bool begins_with(std::wstring_view s, std::wstring_view what) noexcept
{
	if (s.empty() || what.empty() || what.length() > s.length()) {
		return false;
	}
	return wcsncmp(s.data(), what.data(), what.length()) == 0;
}

/// Checks, case insensitive, if the string begins with the given text.
/// @see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strnicmp-wcsnicmp-mbsnicmp-strnicmp-l-wcsnicmp-l-mbsnicmp-l
[[nodiscard]] inline bool begins_withi(std::wstring_view s, std::wstring_view what) noexcept
{
	if (s.empty() || what.empty() || what.length() > s.length()) {
		return false;
	}
	return _wcsnicmp(s.data(), what.data(), what.length()) == 0;
}

/// Checks, case sensitive, if the string ends with the given text.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lstrcmpw
[[nodiscard]] inline bool ends_with(std::wstring_view s, std::wstring_view what) noexcept
{
	if (s.empty() || what.empty() || what.length() > s.length()) {
		return false;
	}
	return lstrcmpW(s.data() + s.length() - what.length(), what.data()) == 0;
}

/// Checks, case insensitive, if the string ends with the given text.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lstrcmpiw
[[nodiscard]] inline bool ends_withi(std::wstring_view s, std::wstring_view what) noexcept
{
	if (s.empty() || what.empty() || what.length() > s.length()) {
		return false;
	}
	return lstrcmpiW(s.data() + s.length() - what.length(), what.data()) == 0;
}

/// Finds index of substring within string, case sensitive.
[[nodiscard]] inline std::optional<size_t> find(
	std::wstring_view haystack, std::wstring_view needle, size_t offset = 0) noexcept
{
	size_t idx = haystack.find(needle, offset);
	if (idx == std::wstring::npos) {
		return std::nullopt;
	}
	return {idx};
}

/// Finds index of substring within string, case insensitive.
[[nodiscard]] inline std::optional<size_t> findi(
	std::wstring_view haystack, std::wstring_view needle, size_t offset = 0)
{
	std::wstring haystack2 = to_upper(haystack);
	std::wstring needle2 = to_upper(needle);
	return str::find(haystack2, needle2, offset); // disambiguation from std::find
}

/// Finds index of substring within string, case sensitive, reverse search.
[[nodiscard]] inline std::optional<size_t> r_find(
	std::wstring_view haystack, std::wstring_view needle, size_t offset = 0) noexcept
{
	size_t idx = haystack.rfind(needle, offset);
	if (idx == std::wstring::npos) {
		return std::nullopt;
	}
	return {idx};
}

/// Finds index of substring within string, case insensitive, reverse search.
[[nodiscard]] inline std::optional<size_t> r_findi(
	std::wstring_view haystack, std::wstring_view needle, size_t offset = 0)
{
	std::wstring haystack2 = to_upper(haystack);
	std::wstring needle2 = to_upper(needle);
	return r_find(haystack2, needle2, offset);
}

/// In-place finds all occurrences of needle, case sensitive, and replaces them all.
/// @return Reference to the same passed string.
inline std::wstring& replace(std::wstring& haystack,
	std::wstring_view needle, std::wstring_view replacement)
{
	if (haystack.empty() || needle.empty()) {
		return haystack;
	}

	std::wstring output;
	size_t base = 0;
	size_t found = 0;

	for (;;) {
		found = haystack.find(needle, found);
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

/// In-place finds all occurrences of needle, case insensitive, and replaces them all.
/// @return Reference to the same passed string.
inline std::wstring& replacei(std::wstring& haystack,
	std::wstring_view needle, std::wstring_view replacement)
{
	if (haystack.empty() || needle.empty()) {
		return haystack;
	}

	std::wstring haystackU = to_upper(haystack);
	std::wstring needleU = to_upper(needle);

	std::wstring output;
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

/// In-place removes any padding zeroes after the string, making size correct.
/// @return Reference to the same passed string.
inline std::wstring& trim_nulls(std::wstring& s)
{
	// When a std::wstring is initialized with any length, possibly to be used as a buffer,
	// the string length may not match the size() method, after the operation.
	// This function fixes this.
	if (!s.empty()) {
		s.resize( lstrlenW(s.c_str()) );
	}
	s.shrink_to_fit();
	return s;
}

/// In-place trims left characters off the string.
/// @return Reference to the same passed string.
inline std::wstring& trim_left(std::wstring& s, wchar_t charToTrim)
{
	size_t i = 0;
	for (; i < s.length(); ++i) {
		if (s[i] != charToTrim) break;
	}
	s.erase(0, i);
	return s;
}

/// In-place trims right characters off the string.
/// @return Reference to the same passed string.
inline std::wstring& trim_right(std::wstring& s, wchar_t charToTrim)
{
	size_t i = s.length();
	for (; i-- > 0; ) {
		if (s[i] != charToTrim) break;
	}
	s.erase(i + 1, s.length() - i - 1);
	return s;
}

/// In-place trims the string.
/// @note Spaces are defined by std::iswspace() function.
/// @see https://en.cppreference.com/w/cpp/string/wide/iswspace
/// @return Reference to the same passed string.
inline std::wstring& trim(std::wstring& s)
{
	if (s.empty()) return s;
	trim_nulls(s);

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

/// Does the string represent a signed int?
/// @see https://en.cppreference.com/w/cpp/string/wide/iswdigit
/// @see https://en.cppreference.com/w/cpp/string/wide/iswblank
[[nodiscard]] inline bool is_int(std::wstring_view s) noexcept
{
	if (s.empty()) return false;
	if (s[0] != L'-' && !std::iswdigit(s[0]) && !std::iswblank(s[0])) return false;
	for (wchar_t ch : s) {
		if (!std::iswdigit(ch) && !std::iswblank(ch)) return false;
	}
	return true;
}

/// Does the string represent an unsigned int?
/// @see https://en.cppreference.com/w/cpp/string/wide/iswdigit
/// @see https://en.cppreference.com/w/cpp/string/wide/iswblank
[[nodiscard]] inline bool is_uint(std::wstring_view s) noexcept
{
	if (s.empty()) return false;
	for (wchar_t ch : s) {
		if (!std::iswdigit(ch) && !std::iswblank(ch)) return false;
	}
	return true;
}

/// Does the string represent a hexadecimal int?
/// @see https://en.cppreference.com/w/cpp/string/wide/iswxdigit
/// @see https://en.cppreference.com/w/cpp/string/wide/iswblank
[[nodiscard]] inline bool is_hex(std::wstring_view s) noexcept
{
	if (s.empty()) return false;
	for (wchar_t ch : s) {
		if (!std::iswxdigit(ch) && !std::iswblank(ch)) return false;
	}
	return true;
}

/// Does the string represent a float?
/// @see https://en.cppreference.com/w/cpp/string/wide/iswdigit
/// @see https://en.cppreference.com/w/cpp/string/wide/iswblank
[[nodiscard]] inline bool is_float(std::wstring_view s) noexcept
{
	if (s.empty()) return false;
	if (s[0] != L'-' && s[0] != L'.' && !std::iswdigit(s[0]) && !std::iswblank(s[0])) return false;

	bool hasDot = false;
	for (wchar_t ch : s) {
		if (ch == L'.') {
			if (hasDot) {
				return false;
			} else {
				hasDot = true;
			}
		} else {
			if (!std::iswdigit(ch) && !std::iswblank(ch)) return false;
		}
	}
	return true;
}

/// Converts number to wstring, adding thousand separator.
/// @return New string.
[[nodiscard]] inline std::wstring num_to_wstring_with_separator(
	int number, wchar_t separator = L',')
{
	std::wstring ret;
	ret.reserve(32); // arbitrary length

	int abso = abs(number);
	BYTE numBlocks = 0;
	while (abso >= 1000) {
		abso = (abso - (abso % 1000)) / 1000;
		++numBlocks;
	}

	abso = abs(number);
	bool firstPass = true;
	do {
		int num = abso % 1000;
		wchar_t buf[8]{};

		if (numBlocks > 0) {
			if (num < 100) lstrcatW(buf, L"0");
			if (num < 10) lstrcatW(buf, L"0");
		}

#pragma warning (disable: 4996)
		_itow(num, buf + lstrlenW(buf), 10);
#pragma warning (default: 4996)

		if (firstPass) {
			firstPass = false;
		} else {
			ret.insert(0, 1, separator);
		}

		ret.insert(0, buf);
		abso = (abso - (abso % 1000)) / 1000;
	} while (numBlocks-- > 0);

	if (number < 0) ret.insert(0, 1, L'-'); // prepend minus signal
	return ret;
}

/// Converts number to wstring, adding thousand separator.
/// @return New string.
[[nodiscard]] inline std::wstring num_to_wstring_with_separator(
	size_t number, wchar_t separator = L',')
{
	return num_to_wstring_with_separator(static_cast<int>(number), separator);
}

/// In-place simple diacritics removal.
///
/// Removed diacritics: ÁáÀàÃãÂâÄäÉéÈèÊêËëÍíÌìÎîÏïÓóÒòÕõÔôÖöÚúÙùÛûÜüÇçÅåÐðÑñØøÝý.
/// @return New string.
inline std::wstring& remove_diacritics(std::wstring& s) noexcept
{
	const wchar_t* diacritics   = L"ÁáÀàÃãÂâÄäÉéÈèÊêËëÍíÌìÎîÏïÓóÒòÕõÔôÖöÚúÙùÛûÜüÇçÅåÐðÑñØøÝý";
	const wchar_t* replacements = L"AaAaAaAaAaEeEeEeEeIiIiIiIiOoOoOoOoOoUuUuUuUuCcAaDdNnOoYy";
	for (wchar_t& ch : s) {
		const wchar_t* pDiac = diacritics;
		const wchar_t* pRepl = replacements;
		while (*pDiac != L'\0') {
			if (ch == *pDiac) ch = *pRepl; // in-place replacement
			++pDiac;
			++pRepl;
		}
	}
	return s;
}

/// In-place reverses the string.
/// @return Reference to the same passed string.
inline std::wstring& reverse(std::wstring& s) noexcept
{
	size_t lim = (s.length() - (s.length() % 2)) / 2;
	for (size_t i = 0; i < lim; ++i) {
		std::swap(s[i], s[s.length() - i - 1]);
	}
	return s;
}

/// Guesses what linebreak is being used on a given string (unknown, N, R, RN or NR).
/// @return First linebreak found, or nullptr if none.
[[nodiscard]] inline const wchar_t* guess_linebreak(std::wstring_view s) noexcept
{
	for (size_t i = 0; i < s.length() - 1; ++i) {
		if (s[i] == L'\r') {
			return s[i + 1] == L'\n' ? L"\r\n" : L"\r";
		} else if (s[i] == L'\n') {
			return s[i + 1] == L'\r' ? L"\n\r" : L"\n";
		}
	}
	return nullptr; // unknown
}

/// Splits the string at the given characters, the characters themselves being removed.
[[nodiscard]] inline std::vector<std::wstring> split(
	std::wstring_view s, std::wstring_view delimiter)
{
	std::vector<std::wstring> ret;
	if (s.empty()) return ret;

	if (delimiter.empty()) {
		ret.emplace_back(s); // one single line
		return ret;
	}

	size_t base = 0, head = 0;

	for (;;) {
		head = s.find(delimiter, head);
		if (head == std::wstring::npos) break;
		ret.emplace_back();
		ret.back().insert(0, s, base, head - base);
		head += delimiter.length();
		base = head;
	}

	ret.emplace_back();
	ret.back().insert(0, s, base, s.length() - base);
	return ret;
}

/// Splits a string line by line.
/// @see wl::str::split()
[[nodiscard]] inline std::vector<std::wstring> split_lines(std::wstring_view s)
{
	return split(s, guess_linebreak(s));
}

/// Splits a zero-delimited multi-string.
/// @param s Ex.: `L"first one\0second one\0third one`.
[[nodiscard]] inline std::vector<std::wstring>
	split_multi_zero(const wchar_t* charArr, size_t lenArr)
{
	if (charArr == nullptr) return {}; // nothing to parse

	// Ltrim zeros.
	size_t lZeros = 0;
	for (size_t i = 0; i < lenArr; ++i) {
		if (charArr[i] == L'\0') {
			++lZeros;
		} else {
			break;
		}
	}
	charArr += lZeros;
	lenArr -= lZeros;
	if (lenArr == 0) return {}; // nothing to parse

	// Rtrim zeros.
	for (size_t i = lenArr; i-- > 0; ) {
		if (charArr[i] != L'\0') {
			lenArr = i + 1;
			break;
		}
	}
	if (lenArr == 0) return {}; // nothing to parse

	// Count how many null-delimited strings.
	size_t numStrings = 1; // because the last one is not counted in the loop below
	for (size_t i = 1; i < lenArr; ++i) {
		if (charArr[i] == L'\0' && charArr[i - 1] != L'\0') {
			++numStrings;
		}
	}

	// Alloc return array of strings.
	std::vector<std::wstring> foundStrings;
	foundStrings.reserve(numStrings);

	// Copy each string.
	std::wstring tmpBuf;
	size_t iBase = 0;
	for (size_t i = 1; i < lenArr; ++i) {
		if (charArr[i] == L'\0' && charArr[i - 1] != L'\0') {
			tmpBuf.insert(0, &charArr[iBase], i - iBase);
			foundStrings.emplace_back(std::move(tmpBuf));
			iBase = i;
			while (charArr[iBase] == L'\0') ++iBase; // points to the next non-null
		}
	}
	tmpBuf.insert(0, &charArr[iBase], lenArr - iBase);
	foundStrings.emplace_back(std::move(tmpBuf));

	return foundStrings;
}

/// Splits a zero-delimited multi-string.
/// @param s Ex.: `L"first one\0second one\0third one`.
[[nodiscard]] inline std::vector<std::wstring>
	split_multi_zero(const std::vector<wchar_t>& s)
{
	return split_multi_zero(&s[0], s.size());
}

/// Splits string into tokens, which may be enclosed in double quotes.
/// @param s Ex.: `L"\"First one\" NoQuoteSecond \"Third one\""`
[[nodiscard]] inline std::vector<std::wstring> split_quoted(std::wstring_view s)
{
	// Count number of strings.
	size_t numStrings = 0;
	const wchar_t* pRun = s.data();
	while (*pRun != L'\0') {
		if (*pRun == L'\"') { // begin of quoted string
			++pRun; // point to 1st char of string
			for (;;) {
				if (*pRun == L'\0') {
					break; // won't compute open-quoted
				} else if (*pRun == L'\"') {
					++pRun; // point to 1st char after closing quote
					++numStrings;
					break;
				}
				++pRun;
			}
		} else if (!std::iswspace(*pRun)) { // 1st char of non-quoted string
			++pRun; // point to 2nd char of string
			while (*pRun != L'\0' && !std::iswspace(*pRun) && *pRun != L'\"') ++pRun; // passed string
			++numStrings;
		} else {
			++pRun; // some white space
		}
	}

	// Alloc return array of strings.
	std::vector<std::wstring> ret;
	ret.reserve(numStrings);

	// Alloc and copy each string.
	pRun = s.data();
	const wchar_t* pBase;
	int i = 0;
	while (*pRun != L'\0') {
		if (*pRun == L'\"') { // begin of quoted string
			++pRun; // point to 1st char of string
			pBase = pRun;
			for (;;) {
				if (*pRun == L'\0') {
					break; // won't compute open-quoted
				} else if (*pRun == L'\"') {
					ret.emplace_back();
					ret.back().insert(0, pBase, pRun - pBase); // copy to buffer
					++i; // next string

					++pRun; // point to 1st char after closing quote
					break;
				}
				++pRun;
			}
		} else if (!std::iswspace(*pRun)) { // 1st char of non-quoted string
			pBase = pRun;
			++pRun; // point to 2nd char of string
			while (*pRun && !std::iswspace(*pRun) && *pRun != L'\"') ++pRun; // passed string

			ret.emplace_back();
			ret.back().insert(0, pBase, pRun - pBase); // copy to buffer
			++i; // next string
		} else {
			++pRun; // some white space
		}
	}

	return ret;
}

}//namespace wl::str