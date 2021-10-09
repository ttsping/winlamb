/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <cstdint>

#define WLDUMMY

#ifndef STRINGIFY
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#endif

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif
