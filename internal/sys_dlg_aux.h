/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <string_view>
#include <system_error>
#include <vector>
#include <Windows.h>
#include <shtypes.h>

namespace _wli::sys_dlg_aux {

inline static HHOOK globalMsgBoxHook;
inline static HWND  globalMsgBoxParent;

[[nodiscard]] inline std::vector<COMDLG_FILTERSPEC>
	build_filters(std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> filters)
{
	std::vector<COMDLG_FILTERSPEC> filterSpec;
	filterSpec.reserve(filters.size());

	for (const std::pair<std::wstring_view, std::wstring_view>& f : filters) {
		filterSpec.push_back({f.first.data(), f.second.data()});
	}
	return filterSpec;
}

}//namespace _wli::sys_dlg_aux