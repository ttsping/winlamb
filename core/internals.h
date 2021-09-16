
#pragma once

namespace core_internals {

// Centralizes all exception handling, displaying the error with MessageBox().
int Lippincott() noexcept;

// Assembles two DWORDs into an UINT64.
inline UINT64 Make64(DWORD lo, DWORD hi) noexcept { return ((UINT64)hi << 32) | lo; }

// Extracts the low-order DWORD out of an UINT64;
inline DWORD Lo64(UINT64 n) noexcept { return n & 0xffff'ffff; }

// Extracts the high-order DWORD out of an UINT64;
inline DWORD Hi64(UINT64 n) noexcept { return (n >> 32) & 0xffff'ffff;  }

}
