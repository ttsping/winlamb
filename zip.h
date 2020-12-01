/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <Windows.h>
#include <ShlObj.h>
#include "com.h"
#include "path.h"

/// Zip file utilities.
///
/// #include <zip.h>
namespace wl::zip {

/// Extracts all files from a zip file.
/// @param zipFile Zip file to have its contents extracted.
/// @param destFolder Folder where the unzipped files will be placed.
inline void extract_all(std::wstring_view zipFile, std::wstring_view destFolder)
{
	if (!path::exists(zipFile)) {
		throw std::invalid_argument("File doesn't exist.");
	}
	if (!path::exists(destFolder)) {
		throw std::invalid_argument("Output directory doesn't exist.");
	}

	// http://social.msdn.microsoft.com/Forums/vstudio/en-US/45668d18-2840-4887-87e1-4085201f4103/visual-c-to-unzip-a-zip-file-to-a-specific-directory
	com::lib comLib{com::lib::init::NOW};

	auto shellDispatch = com::co_create_instance<IShellDispatch>(
		CLSID_Shell, IID_IShellDispatch, CLSCTX_INPROC_SERVER);

	com::variant variZipFilePath;
	variZipFilePath.set_str(zipFile);

	com::ptr<Folder> zippedFile;
	if (HRESULT hr = shellDispatch->NameSpace(variZipFilePath, zippedFile.raw_pptr()); FAILED(hr)) {
		throw std::system_error(hr, std::system_category(),
			"IShellDispatch::NameSpace() failed on zip file name in " __FUNCTION__ "().");
	}

	com::variant variOutFolderPath;
	variOutFolderPath.set_str(destFolder);

	com::ptr<Folder> outFolder;
	if (HRESULT hr = shellDispatch->NameSpace(variOutFolderPath, outFolder.raw_pptr()); FAILED(hr)) {
		throw std::system_error(hr, std::system_category(),
			"IShellDispatch::NameSpace() failed on directory name in " __FUNCTION__ "().");
	}

	com::ptr<FolderItems> filesInside;
	if (HRESULT hr = zippedFile->Items(filesInside.raw_pptr()); FAILED(hr)) {
		throw std::runtime_error("Folder::Items() failed in " __FUNCTION__ "().");
	}

	long fileCount = 0;
	if (HRESULT hr = filesInside->get_Count(&fileCount); FAILED(hr)) {
		throw std::runtime_error("Folder::get_Count() failed in " __FUNCTION__ "().");
	}

	com::variant variItem;
	variItem.set_query_idispatch(filesInside, IID_IDispatch);

	com::variant variOptions;
	variOptions.set_int32(1024 | 512 | 16 | 4); // https://docs.microsoft.com/en-us/windows/win32/shell/folder-copyhere

	if (HRESULT hr = outFolder->CopyHere(variItem, variOptions); FAILED(hr)) {
		throw std::runtime_error("Folder::CopyHere() failed in " __FUNCTION__ "().");
	}
}

}//namespace wl::zip