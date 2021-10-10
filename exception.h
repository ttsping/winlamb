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

class invalid_parameters_exception : public std::runtime_error {
  public:
      invalid_parameters_exception(const char* msg = "") : std::runtime_error(msg) {}
};

#define THROW_API_EXCEPTION(api) throw api_exception(api " failed. @" __FUNCTION__);
#define THROW_INVALID_PARAM_EXCEPTION() throw invalid_parameters_exception("invalid parameter. @" __FUNCTION__);

}  // namespace wl