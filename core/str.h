
#pragma once
#include <string>
#include <string_view>
#include <Windows.h>

namespace core::str {

// Formats the string and calls OutputDebugString().
void Dbg(std::wstring_view format, ...);

// Formats the string; sprintf() wrapper.
[[nodiscard]] std::wstring Format(std::wstring_view format, ...);

// Returns the string message of the given native error code.
[[nodiscard]] std::wstring FormatError(DWORD errCode);

// Simple diacritics removal, in-place.
std::wstring& RemoveDiacritics(std::wstring& s);

// Reverses the string, in-place.
std::wstring& Reverse(std::wstring& s);

// Returns a new string converted to lowercase.
[[nodiscard]] std::wstring ToLower(std::wstring_view s);

// Returns a new string converted to uppercase.
[[nodiscard]] std::wstring ToUpper(std::wstring_view s);

// Trims the string using std::iswspace to validate spaces, in-place.
std::wstring& Trim(std::wstring& s);

// Removes any padding zeroes after the string, making size correct, in-place.
std::wstring& TrimNulls(std::wstring& s);

}
