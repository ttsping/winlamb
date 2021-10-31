/*
 * Part of WinLamb - Win32 API Lambda Lib+rary
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "internal/path_aux.h"
#include "internal/com_ptr.h"
#include "internal/base_priv.h"
#include "str.h"

#pragma comment(lib, "shlwapi.lib")

/// Utilities to file paths.
///
/// #include <path.h>
namespace wl::path {

/// Retrieves the complete folder path from a file path.
/// @return Folder path without trailing backslash.
NODISCARD inline std::wstring dir_from(std::wstring_view filePath) {
    std::wstring ret = filePath.data();
    size_t found = ret.find_last_of(L'\\');
    if (found != std::wstring::npos) {
        ret.resize(found);
    }
    return ret;
}

/// Retrieves the file name from a file path.
NODISCARD inline std::wstring file_from(std::wstring_view filePath) {
    std::wstring ret = filePath.data();
    size_t found = ret.find_last_of(L'\\');
    if (found != std::wstring::npos) {
        ret.erase(0, found + 1);
    }
    return ret;
}

/// In-place removes the trailing backslash of path, if any.
inline std::wstring& trim_backslash(std::wstring& filePath) {
    while (filePath.back() == L'\\') {
        filePath.resize(filePath.length() - 1);
    }
    return filePath;
}

/// Tells if the file has the given extension, case-insensitive.
NODISCARD inline bool has_extension(std::wstring_view filePath, std::wstring_view extension) noexcept {
    if (extension[0] == L'.') {  // extension starts with dot, compare right away
        return str::ends_withi(filePath, extension);
    }

    wchar_t dotExtension[32] = L".";  // arbitrary buffer length
    lstrcatW(dotExtension, extension.data());
    return str::ends_withi(filePath, dotExtension);
}

/// Tells if the file has one the given extensions, case-insensitive.
NODISCARD inline bool has_extension(std::wstring_view filePath, std::initializer_list<std::wstring_view> extensions) noexcept {
    for (std::wstring_view ex : extensions) {
        if (has_extension(filePath, ex)) {
            return true;
        }
    }
    return false;
}

/// In-place changes the extension to the given one, or appends if it has no extension.
inline std::wstring& change_extension(std::wstring& filePath, std::wstring_view newExtension) {
    size_t dotIdx = filePath.find_last_of(L'.');
    if (dotIdx != std::wstring::npos) {  // filePath already has an extension
        filePath.resize(dotIdx + 1);     // truncate after the dot
    } else {                             // filePath doesn't have an extension
        filePath.append(1, L'.');
    }
    filePath.append(newExtension[0] == L'.' ? &newExtension[1] : newExtension);
    return filePath;
}

inline std::wstring append(std::wstring_view basePath, std::wstring_view morePath) {
    std::wstring res{ basePath };
    res.resize(MAX_PATH);
    ::PathAppendW(std::data(res), std::data(morePath));
    str::trim_nulls(res);
    return res;
}

/// Tells if the given file path exists.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesw
NODISCARD inline bool exists(std::wstring_view filePath) noexcept { return GetFileAttributesW(filePath.data()) != INVALID_FILE_ATTRIBUTES; }

/// Tells if two paths are the same.
/// Simple case-insensitive string comparison.
/// @see str::eqi().
NODISCARD inline bool is_same(std::wstring_view path1, std::wstring_view path2) noexcept { return str::eqi(path1, path2); }

/// Tells if a path is a directory.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesw
NODISCARD inline bool is_dir(std::wstring_view anyPath) noexcept { return (GetFileAttributesW(anyPath.data()) & FILE_ATTRIBUTE_DIRECTORY) != 0; }

/// Tells if the path is hidden.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesw
NODISCARD inline bool is_hidden(std::wstring_view anyPath) noexcept { return (GetFileAttributesW(anyPath.data()) & FILE_ATTRIBUTE_HIDDEN) != 0; }

/// Creates a new directory.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createdirectoryw
inline void create_dir(std::wstring_view dirPath) {
    if (CreateDirectoryW(dirPath.data(), nullptr) == FALSE) {
        throw std::system_error(GetLastError(), std::system_category(), "CreateDirectory() failed in create_dir().");
    }
}

/// Create directories, it will create intermediate directories which do not exists.
inline void create_dir_ex(std::wstring_view dirPath) {
    int err = SHCreateDirectory(nullptr, dirPath.data());
    if (err != ERROR_SUCCESS) {
        throw std::system_error(err, std::system_category(), "SHCreateDirectory() failed in create_dir_ex().");
    }
}

/// Deletes a file, or a directory recursively.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-deletefilew
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shfileoperationw
inline void del(std::wstring_view fileOrFolder) {
    if (is_dir(fileOrFolder)) {
        // http://stackoverflow.com/q/1468774/6923555
        wchar_t szDir[MAX_PATH + 1]{};  // +1 for the double null terminate
        lstrcpyW(szDir, fileOrFolder.data());

        SHFILEOPSTRUCTW fos{};
        fos.wFunc = FO_DELETE;
        fos.pFrom = szDir;
        fos.fFlags = FOF_NO_UI;

        if (SHFileOperationW(&fos) != 0) {
            throw std::runtime_error("SHFileOperation() failed to recursively delete directory, unspecified error, in del().");
        }
    } else {
        if (DeleteFileW(fileOrFolder.data()) == FALSE) {
            throw std::system_error(GetLastError(), std::system_category(), "DeleteFile() failed in del().");
        }
    }
}

/// Lists files within a directory according to a pattern,
/// @param pathAndPattern String like "C:\\files\\*.txt". Just "*" will bring all.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfilew
NODISCARD inline std::vector<std::wstring> list_dir(std::wstring_view pathAndPattern) {
    std::vector<std::wstring> files;

    WIN32_FIND_DATA wfd{};
    HANDLE hFind = FindFirstFileW(pathAndPattern.data(), &wfd);
    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND) {
            return files;
        } else {
            throw std::system_error(err, std::system_category(), "FindFirstFile() failed in list_dir().");
        }
    }

    std::wstring pathPat{ pathAndPattern };
    pathPat.erase(pathAndPattern.find_last_of(L'\\'));  // no trailing backslash

    do {
        if (*wfd.cFileName && lstrcmpiW(wfd.cFileName, L".")  // do not add current and parent paths
            && lstrcmpiW(wfd.cFileName, L"..")) {
            files.emplace_back(pathPat);
            files.back().append(L"\\").append(wfd.cFileName);
        }
    } while (FindNextFileW(hFind, &wfd) != FALSE);

    FindClose(hFind);
    return files;
}

/// System path locations that can be retrieved with sys_path().
enum class to { MY_DOCUMENTS, MY_MUSIC, MY_PICTURES, MY_VIDEO, DESKTOP, APP_DATA, LOCAL_APP_DATA, COMMON_APP_DATA, PROGRAM_FILES, PROGRAM_FILES_X86, TEMP };

/// Retrieves a system path.
/// @return Path without trailing backslash.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetfolderpathw
NODISCARD inline std::wstring sys_path(to pathToRetrieve) {
    switch (pathToRetrieve) {
        case to::MY_DOCUMENTS:
            return _wli::sys_path_shell(CSIDL_MYDOCUMENTS);
        case to::MY_MUSIC:
            return _wli::sys_path_shell(CSIDL_MYMUSIC);
        case to::MY_PICTURES:
            return _wli::sys_path_shell(CSIDL_MYPICTURES);
        case to::MY_VIDEO:
            return _wli::sys_path_shell(CSIDL_MYVIDEO);
        case to::DESKTOP:
            return _wli::sys_path_shell(CSIDL_DESKTOPDIRECTORY);
        case to::APP_DATA:
            return _wli::sys_path_shell(CSIDL_APPDATA);
        case to::LOCAL_APP_DATA:
            return _wli::sys_path_shell(CSIDL_LOCAL_APPDATA);
        case to::COMMON_APP_DATA:
            return _wli::sys_path_shell(CSIDL_COMMON_APPDATA);
        case to::PROGRAM_FILES:
            return _wli::sys_path_shell(CSIDL_PROGRAM_FILES);
        case to::PROGRAM_FILES_X86:
            return _wli::sys_path_shell(CSIDL_PROGRAM_FILESX86);
        case to::TEMP:
            return _wli::sys_path_temp();
    }
    return {};  // never reached
}

/// Creates an IShellItem from a string path.
///
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellitem
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-shcreateitemfromparsingname
NODISCARD inline wl::com::ptr<IShellItem> str_to_ishellitem(std::wstring_view fullPath) {
    wl::com::ptr<IShellItem> p;

    if (HRESULT hr = SHCreateItemFromParsingName(fullPath.data(), nullptr, IID_IShellItem, reinterpret_cast<void**>(p.raw_pptr())); FAILED(hr)) {
        throw std::system_error(hr, std::system_category(), "SHCreateItemFromParsingName() failed in " __FUNCTION__ "().");
    }
    return p;
}

/// Extracts the string path from an IShellItem.
///
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellitem
NODISCARD inline std::wstring ishellitem_to_str(wl::com::ptr<IShellItem>& shItem) {
    wchar_t* filePath = nullptr;

    if (HRESULT hr = shItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath); FAILED(hr)) {
        throw std::system_error(hr, std::system_category(), "IShellItem::GetDisplayName() failed in " __FUNCTION__ "().");
    }

    std::wstring retPath = filePath;
    CoTaskMemFree(filePath);
    return retPath;
}

/// Extracts the string paths from an IShellItemArray.
///
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellitemarray
NODISCARD inline std::vector<std::wstring> ishellitemarray_to_strs(wl::com::ptr<IShellItemArray>& shArray) {
    DWORD numPaths = 0;

    if (HRESULT hr = shArray->GetCount(&numPaths); FAILED(hr)) {
        throw std::system_error(hr, std::system_category(), "IShellItemArray::GetCount() failed in " __FUNCTION__ "().");
    }

    std::vector<std::wstring> retPaths;
    retPaths.reserve(numPaths);

    for (DWORD i = 0; i < numPaths; ++i) {
        wl::com::ptr<IShellItem> shItem;
        if (HRESULT hr = shArray->GetItemAt(i, shItem.raw_pptr()); FAILED(hr)) {
            throw std::system_error(hr, std::system_category(), "IShellItemArray::GetItemAt() failed in " __FUNCTION__ "().");
        }
        retPaths.emplace_back(ishellitem_to_str(shItem));
    }
    return retPaths;
}

}  // namespace wl::path