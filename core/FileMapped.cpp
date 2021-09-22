
#include <system_error>
#include "FileMapped.h"
using namespace core;
using std::span;
using std::system_error;
using std::vector;
using std::wstring_view;

constexpr FileMapped::FileMapped(FileMapped&& other)
	: file{std::move(other.file)}, hMap{nullptr}, pMem{nullptr}, sz{0}, readOnly{true}
{
	std::swap(this->hMap, other.hMap);
	std::swap(this->pMem, other.pMem);
	std::swap(this->sz, other.sz);
	std::swap(this->readOnly, other.readOnly);
}

FileMapped& FileMapped::operator=(FileMapped&& other)
{
	this->close();
	this->file = std::move(other.file);
	std::swap(this->hMap, other.hMap);
	std::swap(this->pMem, other.pMem);
	std::swap(this->sz, other.sz);
	std::swap(this->readOnly, other.readOnly);
	return *this;
}

FileMapped::FileMapped(wstring_view filePath, Access access)
	: file{filePath, access == Access::READ ? File::Access::READ_EXISTING : File::Access::RW_EXISTING},
		hMap{nullptr}, pMem{nullptr}, sz{0}, readOnly{access == Access::READ}
{
	this->mapInMemory();
}

void FileMapped::close()
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

vector<BYTE> FileMapped::readChunk(size_t offset, size_t numBytes) const
{
	span<const BYTE> slice = this->hotSpan();
	vector<BYTE> buf(slice.size(), 0x00); // alloc buffer
	memcpy(&buf[0], slice.data(), sizeof(BYTE) * slice.size());
	return buf;
}

void FileMapped::resize(size_t newSize)
{
	UnmapViewOfFile(this->pMem);
	CloseHandle(this->hMap);
	this->file.resize(newSize);
	this->mapInMemory();
}

void FileMapped::mapInMemory()
{
	if (!(this->hMap = CreateFileMapping(this->file.handle(), nullptr,
		this->readOnly ? PAGE_READONLY : PAGE_READWRITE,
		0, 0, nullptr)))
	{
		throw system_error(GetLastError(), std::system_category(), "CreateFileMapping failed");
	}

	if (!(this->pMem = MapViewOfFile(this->hMap,
		this->readOnly ? FILE_MAP_READ : FILE_MAP_WRITE, 0, 0, 0)))
	{
		throw system_error(GetLastError(), std::system_category(), "MapViewOfFile failed");
	}

	this->sz = this->file.size(); // cache file size
}
