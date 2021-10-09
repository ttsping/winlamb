/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <system_error>
#include <minwindef.h>

namespace wl {
class api_exception : public std::system_error {
  public:
    api_exception(const char* api = nullptr, DWORD err = GetLastError()) : std::system_error(err, std::system_category(), api ? api : "") {}
};

#define THROW_API_EXCEPTION(api) throw api_exception(api " failed. @" __FUNCTION__);

}  // namespace wl