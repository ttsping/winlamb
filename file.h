/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <chrono>
#include <string_view>
#include <system_error>
#include <utility>
#include <Windows.h>
#include "bin.h"
#include "str.h"
#include "time.h"

namespace wl {

/// Manages a low-level HANDLE to a file.
/// Calls CloseHandle() in destructor.
///
/// #include <file.h>
class file final {
public:
	/// Time points that can be retrieved from a file.
	struct file_times final {
		/// When the file was created.
		std::chrono::system_clock::time_point creation;
		/// When the file was last accessed.
		std::chrono::system_clock::time_point lastAccess;
		/// When the file was last modified.
		std::chrono::system_clock::time_point lastWrite;
	};

private:
	HANDLE _hFile = nullptr;

public:
	/// Destructor. Calls close().
	~file() { this->close(); }

	/// Default constructor.
	file() = default;

	/// Move constructor.
	file(file&& other) noexcept { this->operator=(std::move(other)); } // movable only

	/// Equality operator.
	[[nodiscard]] bool operator==(const file& other) const noexcept { return this->_hFile == other._hFile; }
	/// Inequality operator.
	[[nodiscard]] bool operator!=(const file& other) const noexcept { return !this->operator==(other); }

	/// Move assignment operator.
	file& operator=(file&& other) noexcept
	{
		this->close();
		std::swap(this->_hFile, other._hFile);
		return *this;
	}

	/// Returns the underlying HFILE handle.
	[[nodiscard]] HANDLE hfile() const noexcept { return this->_hFile; }

	/// Opens an existing file for read access.
	/// Throws an exception if file doesn't exist.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
	file& open_existing_read(std::wstring_view filePath) { return this->_open(filePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING); }

	/// Opens an existing file for read/write access.
	/// Throws an exception if file doesn't exist.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
	file& open_existing_read_write(std::wstring_view filePath) { return this->_open(filePath, GENERIC_READ | GENERIC_WRITE, 0, OPEN_EXISTING); }

	/// Opens a file for read/write access.
	/// If file doesn't exist, it will be created.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
	file& open_or_create_read_write(std::wstring_view filePath) { return this->_open(filePath, GENERIC_READ | GENERIC_WRITE, 0, OPEN_ALWAYS); }

	/// Calls CloseHandle().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
	void close() noexcept
	{
		if (this->_hFile != nullptr) {
			CloseHandle(this->_hFile);
			this->_hFile = nullptr;
		}
	}

	/// Retrieves file creation, last access, and last write times.
	/// The times are in system local time.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletime
	[[nodiscard]] file_times times() const
	{
		FILETIME creation{}, lastAccess{}, lastWrite{};
		if (GetFileTime(this->_hFile, &creation, &lastAccess, &lastWrite) == 0) {
			throw std::system_error(GetLastError(), std::system_category(),
				"GetFileTime() failed in " __FUNCTION__ "().");
		}
		return {
			time::utc_to_local(time::filetime_to_timepoint(creation)),
			time::utc_to_local(time::filetime_to_timepoint(lastAccess)),
			time::utc_to_local(time::filetime_to_timepoint(lastWrite))
		};
	}

	/// Reads all file contents at once.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
	[[nodiscard]] std::vector<BYTE> read_all() const
	{
		std::vector<BYTE> buf(this->size(), 0); // alloc right away
		DWORD bytesRead = 0;

		if (ReadFile(this->_hFile, &buf[0], static_cast<DWORD>(buf.size()),
			&bytesRead, nullptr) == FALSE)
		{
			throw std::system_error(GetLastError(), std::system_category(),
				"ReadFile() failed in " __FUNCTION__ "().");
		}
		return buf;
	}

	/// Reads all file contents at once, and parses as wstring.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
	/// @see bin::parse_str()
	[[nodiscard]] std::wstring read_all_as_string() const
	{
		return bin::parse_str(this->read_all());
	}

	/// Calls SetFilePointerEx() to set internal pointer to begin of the file.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfilepointerex
	const file& rewind() const
	{
		LARGE_INTEGER lnum{};

		if (SetFilePointerEx(this->_hFile, lnum, nullptr, FILE_BEGIN)
			== INVALID_SET_FILE_POINTER)
		{
			throw std::system_error(GetLastError(), std::system_category(),
				"SetFilePointerEx() failed to rewind the file in " __FUNCTION__ "().");
		}
		return *this;
	}

	/// Truncates or expands the file, according to the new size; zero will empty the file.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setendoffile
	const file& set_new_size(size_t numBytes) const
	{
		if (this->size() != numBytes) { // otherwise nothing to do
			return *this;
		}

		LARGE_INTEGER lnum{};
		lnum.QuadPart = numBytes;

		if (SetFilePointerEx(this->_hFile, lnum, nullptr, FILE_BEGIN)
			== INVALID_SET_FILE_POINTER)
		{
			throw std::system_error(GetLastError(), std::system_category(),
				"SetFilePointerEx() failed when setting new file size in " __FUNCTION__ "().");
		}

		if (SetEndOfFile(this->_hFile) == 0) {
			throw std::system_error(GetLastError(), std::system_category(),
				"SetEndOfFile() failed when setting new file size in " __FUNCTION__ "().");
		}

		lnum.QuadPart = 0;

		if (SetFilePointerEx(this->_hFile, lnum, nullptr, FILE_BEGIN) // rewind
			== INVALID_SET_FILE_POINTER)
		{
			throw std::system_error(GetLastError(), std::system_category(),
				"SetFilePointerEx() failed to rewind the file pointer when setting new file size in " __FUNCTION__ "().");
		}

		return *this;
	}

	/// Retrieves the file size with GetFileSizeEx().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	[[nodiscard]] size_t size() const
	{
		LARGE_INTEGER buf{};
		if (GetFileSizeEx(this->_hFile, &buf) == 0) {
			throw std::system_error(GetLastError(), std::system_category(),
				"GetFileSizeEx() failed in " __FUNCTION__ "().");
		}
		return buf.QuadPart;
	}

	/// Writes content to file with WriteFile().
	/// File boundary will be expanded if needed.
	/// Internal file pointer will move forward.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
	const file& write(const BYTE* pData, size_t sz) const
	{
		DWORD dwWritten = 0;
		if (WriteFile(this->_hFile, pData, static_cast<DWORD>(sz), &dwWritten, nullptr) == FALSE) {
			throw std::system_error(GetLastError(), std::system_category(),
				"WriteFile() failed in " __FUNCTION__ "().");
		}
		return *this;
	}

	/// Writes content to file with WriteFile().
	/// File boundary will be expanded if needed.
	/// Internal file pointer will move forward.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
	const file& write(const std::vector<BYTE>& data) const
	{
		return this->write(&data[0], data.size());
	}

private:
	file& _open(std::wstring_view filePath, DWORD desiredAccess, DWORD shareMode,
		DWORD creationDisposition)
	{
		this->close();

		this->_hFile = CreateFileW(filePath.data(), desiredAccess, shareMode,
			nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (this->_hFile == INVALID_HANDLE_VALUE) {
			this->_hFile = nullptr;
			throw std::runtime_error(
				str::unicode_to_ansi(
					str::format(L"CreateFile() failed for \"%s\" in " __FUNCTION__ "().",
						filePath)));
		}
		return *this;
	}
};

}//namespace wl