/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <vector>
#include <Windows.h>
#include <Shlobj.h>
#include "internal/interfaces.h"
#include "internal/sys_dlg_aux.h"
#include "com.h"
#include "path.h"

/// Shows native system dialogs.
///
/// #include <sys_dlg.h>
/// @see @ref ex07
namespace wl::sys_dlg {

/// Ordinary MessageBox(), but centered at parent.
/// @see @ref ex07
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messageboxw
inline int msg_box(const i_window* parent,
	std::wstring_view title, std::wstring_view text, UINT uType = 0)
{
	if (parent == nullptr) {
		throw std::invalid_argument("Cannot call msg_box without a parent.");
	}

	_wli::sys_dlg_aux::globalMsgBoxParent = parent->hwnd();

	_wli::sys_dlg_aux::globalMsgBoxHook = SetWindowsHookExW(WH_CBT,
		[](int code, WPARAM wp, LPARAM lp) noexcept -> LRESULT {
			// http://www.codeguru.com/cpp/w-p/win32/messagebox/print.php/c4541
			if (code == HCBT_ACTIVATE) {
				HWND hMsgbox = reinterpret_cast<HWND>(wp);
				RECT rcMsgbox{}, rcParent{};

				if (hMsgbox != nullptr &&
					_wli::sys_dlg_aux::globalMsgBoxParent != nullptr &&
					GetWindowRect(hMsgbox, &rcMsgbox) != FALSE &&
					GetWindowRect(_wli::sys_dlg_aux::globalMsgBoxParent, &rcParent) != FALSE)
				{
					RECT  rcScreen{};
					POINT pos{};
					SystemParametersInfoW(SPI_GETWORKAREA, 0,
						static_cast<PVOID>(&rcScreen), 0); // size of desktop

					// Adjusted x,y coordinates to message box window.
					pos.x = rcParent.left + (rcParent.right - rcParent.left)
						/ 2 - (rcMsgbox.right - rcMsgbox.left) / 2;
					pos.y = rcParent.top + (rcParent.bottom - rcParent.top)
						/ 2 - (rcMsgbox.bottom - rcMsgbox.top) / 2;

					// Screen out-of-bounds corrections.
					if (pos.x < 0) {
						pos.x = 0;
					} else if (pos.x + (rcMsgbox.right - rcMsgbox.left) > rcScreen.right) {
						pos.x = rcScreen.right - (rcMsgbox.right - rcMsgbox.left);
					}
					if (pos.y < 0) {
						pos.y = 0;
					} else if (pos.y + (rcMsgbox.bottom - rcMsgbox.top) > rcScreen.bottom) {
						pos.y = rcScreen.bottom - (rcMsgbox.bottom - rcMsgbox.top);
					}

					MoveWindow(hMsgbox, pos.x, pos.y,
						rcMsgbox.right - rcMsgbox.left, rcMsgbox.bottom - rcMsgbox.top,
						FALSE);
				}
				UnhookWindowsHookEx(_wli::sys_dlg_aux::globalMsgBoxHook); // release global hook
			}
			return CallNextHookEx(nullptr, code, wp, lp);
		},
		nullptr, GetCurrentThreadId());

	if (_wli::sys_dlg_aux::globalMsgBoxHook == nullptr) {
		throw std::system_error(GetLastError(), std::system_category(),
			"SetWindowsHookEx() failed in " __FUNCTION__ "().");
	}

	return MessageBoxW(parent->hwnd(), text.data(), title.data(), uType);
}

/// Shows the "open file" system dialog, allowing the user to pick up a file path.
/// @param parent Parent window.
/// @param filter Extension filter.
/// Ex.: `{{L"Documents", L"*.doc;*.xls"}, {L"All files", L"*.*"}}`
/// @return Chosen path, or none if user cancelled.
/// @see @ref ex07
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ifileopendialog
[[nodiscard]] inline std::optional<std::wstring> open_single_file(
	const i_parent_window* parent,
	std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> filters)
{
	com::lib comLib{com::lib::init::NOW};
	com::ptr<IFileOpenDialog> fDlg = com::co_create_instance<IFileOpenDialog>(CLSID_FileOpenDialog);

	// Set IFileDialog flags.
	DWORD dwFlags = 0;
	fDlg->GetOptions(&dwFlags);
	fDlg->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST);

	// Set user filters.
	std::vector<COMDLG_FILTERSPEC> filterSpec = _wli::sys_dlg_aux::build_filters(filters);
	fDlg->SetFileTypes(static_cast<UINT>(filterSpec.size()), &filterSpec[0]);

	// Show the open file dialog.
	if (HRESULT hr = fDlg->Show(parent->hwnd());
		hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		return std::nullopt; // user cancelled
	} else if (hr != S_OK) {
		throw std::system_error(hr, std::system_category(),
			"IFileOpenDialog::Show() failed in " __FUNCTION__ "().");
	}

	// Retrieve chosen path.
	com::ptr<IShellItem> shChosen;
	if (HRESULT hr = fDlg->GetResult(shChosen.raw_pptr()); FAILED(hr)) {
		throw std::system_error(hr, std::system_category(),
			"IFileOpenDialog::GetResult() failed in " __FUNCTION__ "().");
	}
	return path::ishellitem_to_str(shChosen);
}

/// Shows the "open file" system dialog, allowing the user to pick up multiple file paths.
/// @param parent Parent window.
/// @param filter Extension filter.
/// Ex.: `{{L"Documents", L"*.doc;*.xls"}, {L"All files", L"*.*"}}`
/// @return Chosen paths, or empty if user cancelled.
/// @see @ref ex07
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ifileopendialog
[[nodiscard]] inline std::vector<std::wstring> open_multiple_files(
	const i_parent_window* parent,
	std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> filters)
{
	com::lib comLib{com::lib::init::NOW};
	com::ptr<IFileOpenDialog> fDlg = com::co_create_instance<IFileOpenDialog>(CLSID_FileOpenDialog);

	// Set IFileDialog flags.
	DWORD dwFlags = 0;
	fDlg->GetOptions(&dwFlags);
	fDlg->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST | FOS_ALLOWMULTISELECT);

	// Set user filters.
	std::vector<COMDLG_FILTERSPEC> filterSpec = _wli::sys_dlg_aux::build_filters(filters);
	fDlg->SetFileTypes(static_cast<UINT>(filterSpec.size()), &filterSpec[0]);

	// Show the open file dialog.
	if (HRESULT hr = fDlg->Show(parent->hwnd());
		hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		return {}; // user cancelled
	} else if (hr != S_OK) {
		throw std::system_error(hr, std::system_category(),
			"IFileOpenDialog::Show() failed in " __FUNCTION__ "().");
	}

	// Retrieve chosen paths.
	com::ptr<IShellItemArray> shArray;
	if (HRESULT hr = fDlg->GetResults(shArray.raw_pptr()); FAILED(hr)) {
		throw std::system_error(hr, std::system_category(),
			"IFileOpenDialog::GetResults() failed in " __FUNCTION__ "().");
	}
	return path::ishellitemarray_to_strs(shArray);
}

/// Shows the "save file" system dialog, allowing the user to pick up a file path.
/// @param parent Parent window.
/// @param filter Extension filter.
/// Ex.: `{{L"Documents", L"*.doc;*.xls"}, {L"All files", L"*.*"}}`
/// @param defaultFolder Initial folder shown in the dialog. Optional.
/// @param defaultFileName Default file name to be displayed in the text box.
/// @return Chosen path, or none if user cancelled.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ifilesavedialog
[[nodiscard]] inline std::optional<std::wstring> save_file(
	const i_parent_window* parent,
	std::initializer_list<std::pair<std::wstring_view, std::wstring_view>> filters,
	std::optional<std::wstring_view> defaultFolder = {},
	std::optional<std::wstring_view> defaultFileName = {})
{
	com::lib comLib{com::lib::init::NOW};
	com::ptr<IFileSaveDialog> fDlg = com::co_create_instance<IFileSaveDialog>(CLSID_FileSaveDialog);

	// Set IFileDialog flags.
	DWORD dwFlags = 0;
	fDlg->GetOptions(&dwFlags);
	fDlg->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);

	// Set user filters.
	std::vector<COMDLG_FILTERSPEC> filterSpec = _wli::sys_dlg_aux::build_filters(filters);
	fDlg->SetFileTypes(static_cast<UINT>(filterSpec.size()), &filterSpec[0]);

	// Set default folder, if any.
	if (defaultFolder.has_value()) {
		com::ptr<IShellItem> shDefFolder =
			path::str_to_ishellitem(path::dir_from(defaultFolder.value()));

		if (HRESULT hr = fDlg->SetFolder(*shDefFolder.raw_pptr()); FAILED(hr)) {
			throw std::system_error(hr, std::system_category(),
				"IFileSaveDialog::SetFolder() failed in " __FUNCTION__ "().");
		}
	}

	// Set default file name, if any.
	if (defaultFileName.has_value()) {
		if (HRESULT hr = fDlg->SetFileName(defaultFileName.value().data());
			FAILED(hr))
		{
			throw std::system_error(hr, std::system_category(),
				"IFileSaveDialog::SetFileName() failed in " __FUNCTION__ "().");
		}
	}

	// Show the save file dialog.
	if (HRESULT hr = fDlg->Show(parent->hwnd());
		hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		return std::nullopt; // user cancelled
	} else if (hr != S_OK) {
		throw std::system_error(hr, std::system_category(),
			"IFileSaveDialog::Show() failed in " __FUNCTION__ "().");
	}

	// Retrieve chosen path.
	com::ptr<IShellItem> shChosen;
	if (HRESULT hr = fDlg->GetResult(shChosen.raw_pptr()); FAILED(hr)) {
		throw std::system_error(hr, std::system_category(),
			"IFileSaveDialog::GetResult() failed in " __FUNCTION__ "().");
	}
	std::wstring chosenPath = path::ishellitem_to_str(shChosen);
	
	// Append default extension, if needed.
	UINT chosenExtIdx = 0;
	if (HRESULT hr = fDlg->GetFileTypeIndex(&chosenExtIdx); FAILED(hr)) {
		throw std::system_error(hr, std::system_category(),
			"IFileSaveDialog::GetFileTypeIndex() failed in " __FUNCTION__ "().");
	}
	--chosenExtIdx; // returns one-based index

	if (std::wstring_view svChosenExt = (filters.begin() + chosenExtIdx)->second;
		!str::eq(svChosenExt, L"*.*"))
	{
		std::wstring chosenExt{svChosenExt}; // copy
		str::trim_left(chosenExt, L'*');
		if (!path::has_extension(chosenPath, chosenExt)) {
			str::trim_right(chosenPath, L'.');
			chosenPath += chosenExt; // if file name doesn't have chosen extension, append it
		}
	}
	return chosenPath;
}

/// Shows the "choose folder" system dialog, allowing the user to pick up a folder path.
/// @param parent Parent window.
/// @return Chosen path without trailing backslash, or none if user cancelled.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ifileopendialog
[[nodiscard]] inline std::optional<std::wstring> choose_folder(const i_parent_window* parent)
{
	com::lib comLib{com::lib::init::NOW};
	com::ptr<IFileOpenDialog> fDlg = com::co_create_instance<IFileOpenDialog>(CLSID_FileOpenDialog);

	// Set IFileDialog flags.
	DWORD dwFlags = 0;
	fDlg->GetOptions(&dwFlags);
	fDlg->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_PICKFOLDERS);

	// Show the choose folder dialog.
	if (HRESULT hr = fDlg->Show(parent->hwnd());
		hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		return std::nullopt; // user cancelled
	} else if (hr != S_OK) {
		throw std::system_error(hr, std::system_category(),
			"IFileOpenDialog::Show() failed in " __FUNCTION__ "().");
	}

	// Retrieve chosen path.
	com::ptr<IShellItem> shChosen;
	if (HRESULT hr = fDlg->GetResult(shChosen.raw_pptr()); FAILED(hr)) {
		throw std::system_error(hr, std::system_category(),
			"IFileOpenDialog::GetResult() failed in " __FUNCTION__ "().");
	}
	return path::ishellitem_to_str(shChosen);
}

}//namespace wl::sys_dlg