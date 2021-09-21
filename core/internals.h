
#pragma once
#include <Windows.h>
#include <CommCtrl.h>

namespace core_internals {

int Lippincott() noexcept;
[[nodiscard]] constexpr UINT64 Make64(DWORD lo, DWORD hi) { return ((UINT64)hi << 32) | lo; }
[[nodiscard]] constexpr DWORD Lo64(UINT64 n) { return n & 0xffff'ffff; }
[[nodiscard]] constexpr DWORD Hi64(UINT64 n) { return (n >> 32) & 0xffff'ffff;  }
[[nodiscard]] HIMAGELIST ShellImageList(int shil);

}
