/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <cstring>
#include <string_view>
#include <system_error>
#include <Windows.h>
#include "bin.h"
#include "file.h"

namespace wl {

/// Manages a memory-mapped file.
///
/// #include <file_mapped.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/memory/file-mapping
class file_mapped final {
private:
	file _file;
	HANDLE _hMap = nullptr;
	void* _pMem = nullptr;
	size_t _sz = 0;
	bool _readOnly = true; // needed by set_new_size()

public:
	/// Destructor. Calls close().
	~file_mapped() { this->close(); }

	/// Default constructor.
	file_mapped() = default;

	/// Move constructor.
	file_mapped(file_mapped&& other) noexcept { this->operator=(std::move(other)); }

	/// Move assignment operator.
	file_mapped& operator=(file_mapped&& other) noexcept
	{
		this->close();
		std::swap(this->_file, other._file);
		std::swap(this->_hMap, other._hMap);
		std::swap(this->_pMem, other._pMem);
		std::swap(this->_sz, other._sz);
		return *this;
	}

	/// Returns a pointer to the mapped memory.
	[[nodiscard]] BYTE* p_mem() const noexcept { return reinterpret_cast<BYTE*>(this->_pMem); }

	/// Returns a pointer past the mapped memory.
	/// It points to the first BYTE beyond the mapped region.
	[[nodiscard]] BYTE* p_past_mem() const noexcept { return p_mem() + this->_sz; }

	/// Returns the size of the mapped memory.
	/// This value is cached.
	[[nodiscard]] size_t size() const noexcept { return this->_sz; }

	/// Opens an existing file for read access.
	/// @see wl::file::open_existing_read()
	file_mapped& open_read(std::wstring_view filePath) { return this->_open(filePath, true); }

	/// Opens an existing file for read/write access.
	/// @see wl::file::open_existing_read_write()
	file_mapped& open_read_write(std::wstring_view filePath) { return this->_open(filePath, false); }

	/// Releases the mapped memory resource.
	void close() noexcept
	{
		if (this->_pMem != nullptr) {
			UnmapViewOfFile(this->_pMem);
			this->_pMem = nullptr;
		}
		if (this->_hMap != nullptr) {
			CloseHandle(this->_hMap);
			this->_hMap = nullptr;
		}
		this->_file.close();
		this->_sz = 0;
		this->_readOnly = true; // not really needed
	}

	/// Truncates or expands the file, which will be unmapped and remapped back into memory.
	/// @see wl::file::set_new_size()
	file_mapped& set_new_size(size_t numBytes)
	{
		UnmapViewOfFile(this->_pMem);
		CloseHandle(this->_hMap);

		this->_file.set_new_size(numBytes);
		return this->_map_into_memory(L"remapping after set new size");
	}

	/// Gets a copy of the file content.
	/// @param offset Index where to start the copy.
	/// @param numBytes Number of bytes to be copied, or -1 to copy until the end of file.
	[[nodiscard]] std::vector<BYTE> read(size_t offset = 0, size_t numBytes = -1) const
	{
		std::vector<BYTE> buf;
		this->read_to_buffer(buf, offset, numBytes);
		return buf;
	}

	/// Gets a copy of the file content, parsed as wstring.
	/// @param offset Index where to start the copy.
	/// @param numBytes Number of bytes to be copied, or -1 to copy until the end of file.
	/// @see bin::parse_str()
	[[nodiscard]] std::wstring read_as_string(size_t offset = 0, size_t numBytes = -1) const
	{
		return bin::parse_str(this->read(offset, numBytes));
	}

	/// Copies file content into a buffer.
	/// @param buf Buffer to receive the data.
	/// @param offset Index where to start the copy.
	/// @param numBytes Number of bytes to be copied, or -1 to copy until the end of file.
	const file_mapped& read_to_buffer(std::vector<BYTE>& buf,
		size_t offset = 0, size_t numBytes = -1) const
	{
		if (offset >= this->size()) {
			throw std::invalid_argument("Offset is beyond end of file in " __FUNCTION__ "().");
		} else if (numBytes == -1 || offset + numBytes > this->size()) {
			numBytes = this->size() - offset; // avoid reading beyond EOF
		}

		buf.resize(numBytes);
		std::memcpy(&buf[0], this->p_mem() + offset, numBytes * sizeof(BYTE));
		return *this;
	}

private:
	file_mapped& _open(std::wstring_view filePath, bool readOnly)
	{
		this->close();
		this->_readOnly = readOnly;

		if (readOnly) {
			this->_file.open_existing_read(filePath);
		} else {
			this->_file.open_existing_read_write(filePath);
		}

		return this->_map_into_memory(filePath);
	}

	file_mapped& _map_into_memory(std::wstring_view filePath)
	{
		// Mapping into memory.
		this->_hMap = CreateFileMappingW(this->_file.hfile(), nullptr,
			this->_readOnly ? PAGE_READONLY : PAGE_READWRITE, 0, 0, nullptr);
		if (this->_hMap == nullptr) {
			throw std::system_error(GetLastError(), std::system_category(),
				str::unicode_to_ansi(
					str::format(L"CreateFileMapping() failed for \"%s\" in " __FUNCTION__ "().",
						filePath)));
		}

		// Get pointer to data block.
		this->_pMem = MapViewOfFile(this->_hMap,
			this->_readOnly ? FILE_MAP_READ : FILE_MAP_WRITE, 0, 0, 0);
		if (this->_pMem == nullptr) {
			throw std::system_error(GetLastError(), std::system_category(),
				str::unicode_to_ansi(
					str::format(L"MapViewOfFile() failed for \"%s\" in " __FUNCTION__ "().",
						filePath)));
		}

		// Cache file size.
		this->_sz = this->_file.size();

		return *this;
	}
};

}//namespace wl