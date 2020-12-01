/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <string>
#include <Windows.h>

namespace _wli::bin_aux {

// Parses an unsigned 16-bit int, big-endian.
[[nodiscard]] inline WORD parse_uint16_be(const BYTE* data) noexcept
{
	return (data[0] << 8) | data[1];
}

// Parses an unsigned 16-bit int, little-endian.
[[nodiscard]] inline WORD parse_uint16_le(const BYTE* data) noexcept
{
	return data[0] | (data[1] << 8);
}

// Parses an unsigned 32-bit int, big-endian.
[[nodiscard]] inline DWORD parse_uint32_be(const BYTE* data) noexcept
{
	return (data[0] << 24)
		| (data[1] << 16)
		| (data[2] << 8)
		| data[3];
}

// Parses an unsigned 32-bit int, little-endian.
[[nodiscard]] inline DWORD parse_uint32_le(const BYTE* data) noexcept
{
	return data[0]
		| (data[1] << 8)
		| (data[2] << 16)
		| (data[3] << 24);
}

// Parses binary ANSI string into Unicode wstring.
[[nodiscard]] inline std::wstring str_from_ansi(const BYTE* data, size_t sz)
{
	std::wstring ret;
	if (data != nullptr && sz > 0) {
		ret.resize(sz, L'\0'); // make room for the whole string already
		for (size_t i = 0; i < sz; ++i) {
			if (data[i] == 0x00) { // found terminating null in the middle of data, stop processing
				ret.resize(i);
				ret.shrink_to_fit();
				return ret;
			}
			ret[i] = static_cast<wchar_t>(data[i]); // raw conversion
		}
	}
	return ret; // data didn't have a terminating null in the middle of it
}

// Parses binary codepaged data into Unicode wstring with MultiByteToWideChar().
[[nodiscard]] inline std::wstring str_from_code_page(const BYTE* data, size_t sz, UINT codePage)
{
	std::wstring ret;
	if (data != nullptr && sz > 0) {
		int neededLen = MultiByteToWideChar(codePage, 0, reinterpret_cast<const char*>(data),
			static_cast<int>(sz), nullptr, 0);
		ret.resize(neededLen);
		MultiByteToWideChar(codePage, 0, reinterpret_cast<const char*>(data),
			static_cast<int>(sz), &ret[0], neededLen);
		ret.resize( lstrlenW(ret.c_str()) ); // trim_nulls()
		ret.shrink_to_fit();
	}
	return ret;
}

// Parses binary UTF-16 (BE or LE) data into Unicode string.
[[nodiscard]] inline std::wstring str_from_utf16(const BYTE* data, size_t sz, bool isBigEndian)
{
	if (sz % 2 == 1) {
		--sz; // we have an odd number of bytes, discard last one
	}

	std::wstring buf;
	buf.reserve(sz / 2);

	for (size_t i = 0; i < sz; i += 2) {
		buf.append(1, isBigEndian
			? parse_uint16_be(&data[i])
			: parse_uint16_le(&data[i]));
	}
	return buf;
}

}//namespace _wli::bin_aux