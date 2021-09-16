
#include <system_error>
#include "FileMapped.h"
using namespace core;
using std::span;
using std::vector;
using std::wstring_view;

FileMapped& FileMapped::operator=(FileMapped&& other) noexcept
{
	this->close();
	std::swap(this->file, other.file);
	std::swap(this->hMap, other.hMap);
	std::swap(this->pMem, other.pMem);
	return *this;
}

void FileMapped::close() noexcept
{
	if (this->pMem) {
		UnmapViewOfFile(this->pMem);
		this->pMem = nullptr;
	}
	if (this->hMap) {
		CloseHandle(this->hMap);
		this->hMap = nullptr;
	}
	this->file.close();
	this->sz = 0;
	this->readOnly = false;
}

void FileMapped::open(wstring_view filePath, Access access)
{
	this->close();
	this->file.open(filePath, access == Access::READ
		? File::Access::READ_EXISTING
		: File::Access::RW_EXISTING);
	this->readOnly = access == Access::READ;
	this->mapInMemory();
}

void FileMapped::resize(UINT64 newSize)
{
	UnmapViewOfFile(this->pMem);
	CloseHandle(this->hMap);
	this->file.resize(newSize);
	this->mapInMemory();
}

void FileMapped::mapInMemory()
{
	if (!(this->hMap = CreateFileMappingW(this->file.handle(), nullptr,
		this->readOnly ? PAGE_READONLY : PAGE_READWRITE,
		0, 0, nullptr)))
	{
		throw std::system_error(GetLastError(), std::system_category(), "CreateFileMappingW failed");
	}

	if (!(this->pMem = MapViewOfFile(this->hMap,
		this->readOnly ? FILE_MAP_READ : FILE_MAP_WRITE, 0, 0, 0)))
	{
		throw std::system_error(GetLastError(), std::system_category(), "MapViewOfFile failed");
	}
}

vector<BYTE> FileMapped::readChunk(UINT64 offset, UINT64 numBytes) const
{
	span<const BYTE> slice = this->hotSpan();
	vector<BYTE> buf(slice.size(), 0x00); // alloc buffer
	memcpy(&buf[0], slice.data(), sizeof(BYTE) * slice.size());
	return buf;
}
