/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <cstring>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <Windows.h>
#include "internal/bin_aux.h"
#include "internal/byte_view.h"

/// Utilities for binary/number/string conversions.
///
/// #include <bin.h>
namespace wl::bin {

/// Possible string encodings.
enum class encoding {
	/// Unknown encoding.
	UNKNOWN,
	/// ANSI encoding.
	ANSI,
	/// Windows-1252 encoding.
	WIN1252,
	/// UTF-8 encoding.
	UTF8,
	/// UTF-16 big-endian encoding.
	UTF16BE,
	/// UTF-16 little-endian encoding.
	UTF16LE,
	/// UTF-32 big-endian encoding.
	UTF32BE,
	/// UTF-32 little-endian encoding.
	UTF32LE,
	/// Standard Compression Scheme for Unicode encoding.
	SCSU,
	/// Binary Ordered Compression for Unicode encoding.
	BOCU1
};

/// Encoding information of a string.
///
/// #include <bin.h>
struct encoding_info final {
	/// The type of encoding.
	encoding encType = encoding::UNKNOWN;
	/// Size of the byte order mark.
	size_t bomSize = 0;
};

/// Guesses the wl::bin::encoding_info of binary string data.
/// @see https://en.wikipedia.org/wiki/Byte_order_mark
[[nodiscard]] inline encoding_info guess_encoding(const BYTE* data, size_t sz) noexcept
{
	auto match = [data, sz](const BYTE* pBom, int szBom) noexcept -> bool {
		return (sz >= static_cast<size_t>(szBom)) &&
			std::memcmp(data, pBom, sizeof(BYTE) * szBom) == 0;
	};

	BYTE utf8[] = {0xef, 0xbb, 0xbf}; // UTF-8 BOM
	if (match(utf8, 3)) return {encoding::UTF8, 3}; // BOM size in bytes

	BYTE utf16be[] = {0xfe, 0xff};
	if (match(utf16be, 2)) return {encoding::UTF16BE, 2};

	BYTE utf16le[] = {0xff, 0xfe};
	if (match(utf16le, 2)) return {encoding::UTF16LE, 2};

	BYTE utf32be[] = {0x00, 0x00, 0xfe, 0xff};
	if (match(utf32be, 4)) return {encoding::UTF32BE, 4};

	BYTE utf32le[] = {0xff, 0xfe, 0x00, 0x00};
	if (match(utf32le, 4)) return {encoding::UTF32LE, 4};

	BYTE scsu[] = {0x0e, 0xfe, 0xff};
	if (match(scsu, 3)) return {encoding::SCSU, 3};

	BYTE bocu1[] = {0xfb, 0xee, 0x28};
	if (match(bocu1, 3)) return {encoding::BOCU1, 3};

	// No BOM found, guess UTF-8 without BOM, or Windows-1252 (superset of ISO-8859-1).
	bool canBeWin1252 = false;
	for (size_t i = 0; i < sz; ++i) {
		if (data[i] > 0x7f) { // 127
			canBeWin1252 = true;
			if (i <= sz - 2 && (
				(data[i] == 0xc2 && (data[i+1] >= 0xa1 && data[i+1] <= 0xbf)) || // http://www.utf8-chartable.de
				(data[i] == 0xc3 && (data[i+1] >= 0x80 && data[i+1] <= 0xbf)) ))
			{
				return {encoding::UTF8, 0}; // UTF-8 without BOM
			}
		}
	}
	return {(canBeWin1252 ? encoding::WIN1252 : encoding::ANSI), 0};
}

/// Guesses the wl::bin::encoding_info of binary string data.
/// @see https://en.wikipedia.org/wiki/Byte_order_mark
[[nodiscard]] inline encoding_info guess_encoding(const std::vector<BYTE>& data) noexcept { return guess_encoding(&data[0], data.size()); }
/// Guesses the wl::bin::encoding_info of binary string data.
/// @see https://en.wikipedia.org/wiki/Byte_order_mark
[[nodiscard]] inline encoding_info guess_encoding(byte_view data) noexcept { return guess_encoding(&data[0], data.size()); }

/// Parses the binary data into string, guessing the wl::bin::encoding_info.
[[nodiscard]] inline std::wstring parse_str(const BYTE* data, size_t sz)
{
	if (data == nullptr || sz == 0) return {}; // nothing to parse

	encoding_info fileEnc = guess_encoding(data, sz);
	data += fileEnc.bomSize; // skip BOM, if any

	switch (fileEnc.encType) {
		case encoding::UNKNOWN:
		case encoding::ANSI:    return _wli::bin_aux::str_from_ansi(data, sz);
		case encoding::WIN1252: return _wli::bin_aux::str_from_code_page(data, sz, 1252);
		case encoding::UTF8:    return _wli::bin_aux::str_from_code_page(data, sz, CP_UTF8);
		case encoding::UTF16BE: return _wli::bin_aux::str_from_utf16(data, sz, true);
		case encoding::UTF16LE: return _wli::bin_aux::str_from_utf16(data, sz, false);
		case encoding::UTF32BE: throw std::invalid_argument("UTF-32 big endian: encoding not implemented in " __FUNCTION__ "().");
		case encoding::UTF32LE: throw std::invalid_argument("UTF-32 little endian: encoding not implemented in " __FUNCTION__ "().");
		case encoding::SCSU:    throw std::invalid_argument("Standard compression scheme for Unicode: encoding not implemented in " __FUNCTION__ "().");
		case encoding::BOCU1:   throw std::invalid_argument("Binary ordered compression for Unicode: encoding not implemented in " __FUNCTION__ "().");
		default:                throw std::invalid_argument("Unknown encoding in " __FUNCTION__ "().");
	}
}

/// Parses binary data into string, guessing the wl::bin::encoding_info.
[[nodiscard]] inline std::wstring parse_str(const std::vector<BYTE>& data) { return parse_str(&data[0], data.size()); }
/// Parses binary data into string, guessing the wl::bin::encoding_info.
[[nodiscard]] inline std::wstring parse_str(byte_view data) { return parse_str(&data[0], data.size()); }

/// Parses binary data into uint16, big-endian.
[[nodiscard]] inline WORD parse_uint16_be(const BYTE* data) noexcept { return _wli::bin_aux::parse_uint16_be(data); }
/// Parses binary data into uint16, big-endian.
[[nodiscard]] inline WORD parse_uint16_be(const std::vector<BYTE>& data) noexcept { return _wli::bin_aux::parse_uint16_be(&data[0]); }
/// Parses binary data into uint16, big-endian.
[[nodiscard]] inline WORD parse_uint16_be(byte_view data) noexcept { return _wli::bin_aux::parse_uint16_be(&data[0]); }

/// Parses binary data into uint16, little-endian.
[[nodiscard]] inline WORD parse_uint16_le(const BYTE* data) noexcept { return _wli::bin_aux::parse_uint16_le(data); }
/// Parses binary data into uint16, little-endian.
[[nodiscard]] inline WORD parse_uint16_le(const std::vector<BYTE>& data) noexcept { return _wli::bin_aux::parse_uint16_le(&data[0]); }
/// Parses binary data into uint16, little-endian.
[[nodiscard]] inline WORD parse_uint16_le(byte_view data) noexcept { return _wli::bin_aux::parse_uint16_le(&data[0]); }

/// Parses binary data into uint32, big-endian.
[[nodiscard]] inline DWORD parse_uint32_be(const BYTE* data) noexcept { return _wli::bin_aux::parse_uint32_be(data); }
/// Parses binary data into uint32, big-endian.
[[nodiscard]] inline DWORD parse_uint32_be(const std::vector<BYTE>& data) noexcept { return _wli::bin_aux::parse_uint32_be(&data[0]); }
/// Parses binary data into uint32, big-endian.
[[nodiscard]] inline DWORD parse_uint32_be(byte_view data) noexcept { return _wli::bin_aux::parse_uint32_be(&data[0]); }

/// Parses binary data into uint32, little-endian.
[[nodiscard]] inline DWORD parse_uint32_le(const BYTE* data) noexcept { return _wli::bin_aux::parse_uint32_le(data); }
/// Parses binary data into uint32, little-endian.
[[nodiscard]] inline DWORD parse_uint32_le(const std::vector<BYTE>& data) noexcept { return _wli::bin_aux::parse_uint32_le(&data[0]); }
/// Parses binary data into uint32, little-endian.
[[nodiscard]] inline DWORD parse_uint32_le(byte_view data) noexcept { return _wli::bin_aux::parse_uint32_le(&data[0]); }

/// Writes uint16 into binary buffer, big-endian.
inline void put_uint16_be(BYTE* dest, WORD n) noexcept
{
	dest[0] = (n & 0xff00) >> 8;
	dest[1] = n & 0xff;
}
/// Appends uint16 into buffer, big-endian.
inline void put_uint16_be(std::vector<BYTE>& dest, WORD n)
{
	dest.insert(dest.end(), 2, 0x00);
	put_uint16_be(&dest[dest.size() - 2], n);
}

/// Writes uint16 into binary buffer, little-endian.
inline void put_uint16_le(BYTE* dest, WORD n) noexcept
{
	dest[0] = n & 0xff;
	dest[1] = (n & 0xff00) >> 8;
}
/// Appends uint16 into buffer, little-endian.
inline void put_uint16_le(std::vector<BYTE>& dest, WORD n)
{
	dest.insert(dest.end(), 2, 0x00);
	put_uint16_le(&dest[dest.size() - 2], n);
}

/// Writes uint32 into binary buffer, big-endian.
inline void put_uint32_be(BYTE* dest, DWORD n) noexcept
{
	dest[0] = static_cast<BYTE>((n & 0xff00'0000) >> 24);
	dest[1] = static_cast<BYTE>((n & 0xff'0000) >> 16);
	dest[2] = static_cast<BYTE>((n & 0xff00) >> 8);
	dest[3] = static_cast<BYTE>(n & 0xff);
}
/// Appends uint32 into buffer, big-endian.
inline void put_uint32_be(std::vector<BYTE>& dest, DWORD n)
{
	dest.insert(dest.end(), 4, 0x00);
	put_uint32_be(&dest[dest.size() - 4], n);
}

/// Writes uint32 into binary buffer, little-endian.
inline void put_uint32_le(BYTE* dest, DWORD n) noexcept
{
	dest[0] = static_cast<BYTE>(n & 0xff);
	dest[1] = static_cast<BYTE>((n & 0xff00) >> 8);
	dest[2] = static_cast<BYTE>((n & 0xff'0000) >> 16);
	dest[3] = static_cast<BYTE>((n & 0xff00'0000) >> 24);
}
/// Appends uint32 into buffer, little-endian.
inline void put_uint32_le(std::vector<BYTE>& dest, DWORD n)
{
	dest.insert(dest.end(), 4, 0x00);
	put_uint32_le(&dest[dest.size() - 4], n);
}

/// Converts a string into UTF-8 binary data with WideCharToMultiByte().
/// @see https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
inline std::vector<BYTE> str_to_utf8(std::wstring_view s)
{
	std::vector<BYTE> ret;

	if (!s.empty()) {
		int neededLen = WideCharToMultiByte(CP_UTF8, 0,
			s.data(), static_cast<int>(s.length()),
			nullptr, 0, nullptr, 0);
		ret.resize(neededLen);

		WideCharToMultiByte(CP_UTF8, 0,
			s.data(), static_cast<int>(s.length()),
			reinterpret_cast<char*>(&ret[0]),
			neededLen, nullptr, nullptr);
	}
	return ret;
}

/// Swaps the two bytes of an uint16. This is equivalent to transforming
/// big-endian into little-endian, and vice-versa.
inline WORD swap_bytes(WORD n)
{
	BYTE lo = LOBYTE(n), hi = HIBYTE(n);
	return (lo << 8) | hi;
}

}//namespace wl::bin