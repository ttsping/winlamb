
#pragma once
#include <string>
#include <string_view>
#include <Windows.h>

namespace core::str {

void Dbg(std::wstring_view format, ...);
[[nodiscard]] bool EndsWith(std::wstring_view s, std::wstring_view ending);
[[nodiscard]] bool EndsWithI(std::wstring_view s, std::wstring_view ending);
[[nodiscard]] bool EqI(std::wstring_view s1, std::wstring_view s2);
[[nodiscard]] std::wstring Format(std::wstring_view format, ...);
std::wstring& RemoveDiacritics(std::wstring& s);
std::wstring& Reverse(std::wstring& s);
[[nodiscard]] bool StartsWith(std::wstring_view s, std::wstring_view start);
[[nodiscard]] bool StartsWithI(std::wstring_view s, std::wstring_view start);
[[nodiscard]] std::wstring ToLower(std::wstring_view s);
[[nodiscard]] std::wstring ToUpper(std::wstring_view s);
std::wstring& Trim(std::wstring& s);
std::wstring& TrimNulls(std::wstring& s);

}
