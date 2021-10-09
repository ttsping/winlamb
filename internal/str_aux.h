/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <string>
#include "base_priv.h"

namespace _wli::str_aux {

namespace traits {

template<typename T>
struct is_stdastring : std::false_type {};

template<>
struct is_stdastring<std::string> : std::true_type {};

template<>
struct is_stdastring<std::string_view> : std::true_type {};

template<typename T>
inline constexpr bool is_stdastring_v = is_stdastring<std::remove_cv_t<T>>::value;

template<typename T>
inline constexpr bool is_castring_v = std::is_same_v<T, const char*> || std::is_same_v<T, char*>;

template<typename T>
inline constexpr bool is_astring_v = is_stdastring_v<T> || is_castring_v<T>;

template<typename T>
struct is_stdwstring : std::false_type {};

template<>
struct is_stdwstring<std::wstring> : std::true_type {};

template<>
struct is_stdwstring<std::wstring_view> : std::true_type {};

template<typename T>
inline constexpr bool is_stdwstring_v = is_stdwstring<std::remove_cv_t<T>>::value;

template<typename T>
inline constexpr bool is_cwstring_v = std::is_same_v<T, const wchar_t*> || std::is_same_v<T, wchar_t*>;

template<typename T>
inline constexpr bool is_wstring_v = is_stdwstring_v<T> || is_cwstring_v<T>;

template<typename T, std::enable_if_t<is_stdastring_v<T>>* = nullptr>
inline const char* get_ptr(const T& s) { return std::data(s); }

template<typename T, std::enable_if_t<is_castring_v<T>>* = nullptr>
inline const char* get_ptr(const T& s) { return s; }

template<typename T, std::enable_if_t<is_astring_v<T>>* = nullptr>
inline size_t length(const T& s) { return lstrlenA(get_ptr(s)); }

template<typename T, std::enable_if_t<is_wstring_v<T>>* = nullptr>
inline size_t length(const T& s) { return lstrlenW(get_ptr(s)); }

template<typename T, std::enable_if_t<is_astring_v<T>>* = nullptr>
inline size_t format(char* buff, size_t buff_len, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int r = vsnprintf(buff, buff_len, fmt, args);
    va_end(args);
    return size_t(r);
}

template<typename T, std::enable_if_t<is_wstring_v<T>>* = nullptr>
inline size_t format(wchar_t* buff, size_t buff_len, const wchar_t* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int r = vswprintf(buff, buff_len, fmt, args);
    va_end(args);
    return size_t(r);
}
}  // namespace traits

template<typename T>
NODISCARD FORCEINLINE T format_raw_arg(T val) noexcept {
    static_assert(!std::is_same_v<T, std::optional<std::string>>, "In str::format(), call value() to retrieve the string from the optional.");
    static_assert(!std::is_same_v<T, std::optional<std::wstring>>, "In str::format(), call value() to retrieve the wstring from the optional.");
    return val;
}

NODISCARD FORCEINLINE const char* format_raw_arg(const std::string& val) noexcept { return std::data(val); }

NODISCARD FORCEINLINE const char* format_raw_arg(std::string_view val) noexcept { return std::data(val); }

NODISCARD FORCEINLINE const wchar_t* format_raw_arg(const std::wstring& val) noexcept {
    return val.c_str();  // so format() will also accept const wstring& in formatting list
}

NODISCARD FORCEINLINE const wchar_t* format_raw_arg(std::wstring_view val) noexcept {
    return val.data();  // so format() will also accept wstring_view in formatting list
}

template<typename T, typename CharT = typename T::char_type, typename... Args>
inline T format_raw(const CharT* fmt, const Args&... args) {
    // https://msdn.microsoft.com/en-us/magazine/dn913181.aspx
    // https://stackoverflow.com/a/514921/6923555
    size_t len = traits::format<T>(nullptr, 0, fmt, format_raw_arg(args)...);
    T ret(len + 1, L'\0');  // room for terminating null
    traits::format<T>(std::data(ret), len + 1, fmt, format_raw_arg(args)...);
    ret.resize(len);  // remove terminating null
    return ret;
}

}  // namespace _wli::str_aux