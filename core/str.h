
#pragma once
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <Windows.h>

namespace core::str {

void Dbg(std::wstring_view format, ...);
[[nodiscard]] bool EndsWith(std::wstring_view s, std::wstring_view ending);
[[nodiscard]] bool EndsWithI(std::wstring_view s, std::wstring_view ending);
[[nodiscard]] bool EqI(std::wstring_view s1, std::wstring_view s2);
[[nodiscard]] std::optional<size_t> FindSubstr(std::wstring_view haystack, std::wstring_view needle, size_t offset = 0);
[[nodiscard]] std::optional<size_t> FindSubstrI(std::wstring_view haystack, std::wstring_view needle, size_t offset = 0);
[[nodiscard]] std::optional<size_t> FindSubstrRev(std::wstring_view haystack, std::wstring_view needle, size_t offset = 0);
[[nodiscard]] std::optional<size_t> FindSubstrRevI(std::wstring_view haystack, std::wstring_view needle, size_t offset = 0);
[[nodiscard]] std::wstring Format(std::wstring_view format, ...);
[[nodiscard]] std::optional<const wchar_t*> GetLineBreak(std::wstring_view s);
[[nodiscard]] std::wstring Parse(std::span<const BYTE> src);
std::wstring& RemoveDiacritics(std::wstring& s);
std::wstring& Replace(std::wstring& haystack, std::wstring_view needle, std::wstring_view replacement);
std::wstring& ReplaceI(std::wstring& haystack, std::wstring_view needle, std::wstring_view replacement);
std::wstring& Reverse(std::wstring& s);
[[nodiscard]] std::vector<std::wstring> Split(std::wstring_view s, std::wstring_view delimiter, std::optional<size_t> maxPieces = std::nullopt, bool keepBlanks = false);
[[nodiscard]] std::vector<std::wstring> SplitLines(std::wstring_view s);
[[nodiscard]] bool StartsWith(std::wstring_view s, std::wstring_view start);
[[nodiscard]] bool StartsWithI(std::wstring_view s, std::wstring_view start);
[[nodiscard]] std::wstring ToLower(std::wstring_view s);
[[nodiscard]] std::wstring ToUpper(std::wstring_view s);
std::wstring& Trim(std::wstring& s);
std::wstring& TrimNulls(std::wstring& s);

enum class Encoding { UNKNOWN, ASCII, WIN1252, UTF8, UTF16BE, UTF16LE, UTF32BE, UTF32LE, SCSU, BOCU1 };

struct EncodingInfo final {
	Encoding encType = Encoding::UNKNOWN;
	WORD bomSize = 0;
};

[[nodiscard]] EncodingInfo GetEncoding(std::span<const BYTE> src);

}
