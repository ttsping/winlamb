
#pragma once
#include <Windows.h>

namespace core_internals {

const WM_UI_THREAD = WM_APP + 0x3fff;

// Centralizes all exception handling, displaying the error with MessageBox().
int Lippincott() noexcept;

// Assembles two DWORDs into an UINT64.
[[nodiscard]] constexpr UINT64 Make64(DWORD lo, DWORD hi) { return ((UINT64)hi << 32) | lo; }

// Extracts the low-order DWORD out of an UINT64;
[[nodiscard]] constexpr DWORD Lo64(UINT64 n) { return n & 0xffff'ffff; }

// Extracts the high-order DWORD out of an UINT64;
[[nodiscard]] constexpr DWORD Hi64(UINT64 n) { return (n >> 32) & 0xffff'ffff;  }

}
