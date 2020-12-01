/*
* Part of WinLamb - Win32 API Lambda Library
* https://github.com/rodrigocfd/winlamb
* This library is released under the MIT License.
*/

#pragma once
#include <stdexcept>
#include <string>
#include <system_error>
#include <Windows.h>
#include <ShlObj.h>

namespace _wli {

[[nodiscard]] inline std::wstring sys_path_shell(int clsId)
{
	wchar_t buf[MAX_PATH + 1]{};

	if (HRESULT hr = SHGetFolderPathW(nullptr, clsId, nullptr, 0, buf); // won't have trailing backslash
		FAILED(hr))
	{
		throw std::runtime_error("SHGetFolderPath() failed in " __FUNCTION__ "().");
	}
	return {buf};
}

[[nodiscard]] inline std::wstring sys_path_temp()
{
	wchar_t buf[MAX_PATH + 1]{};
	if (GetTempPathW(ARRAYSIZE(buf), buf) == 0) { // will have trailing backslash here
		throw std::system_error(GetLastError(), std::system_category(),
			"GetTempPath() failed in " __FUNCTION__ "().");
	}
	std::wstring ret{buf};
	ret.resize(ret.length() - 1); // remove trailing backslash
	return ret;
}

}//namespace _wli