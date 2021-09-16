
#pragma once
#include <span>
#include <string_view>
#include <vector>
#include <Windows.h>
#include "File.h"

namespace core {

// Encapsulates a memory-mapped file.
class FileMapped final {
private:
	File file;
	HANDLE hMap = nullptr;
	void* pMem = nullptr;
	UINT64 sz = 0;
	bool readOnly = false;

public:
	enum class Access { READ, RW };

	~FileMapped() { this->close(); }

	FileMapped() = default;
	FileMapped(std::wstring_view filePath, Access access) { this->open(filePath, access); }
	FileMapped(FileMapped&& other) noexcept { this->operator=(std::move(other)); }
	FileMapped& operator=(FileMapped&& other) noexcept;

	void close() noexcept;
	void open(std::wstring_view filePath, Access access);
	[[nodiscard]] UINT64 size() const { return this->sz; }
	void resize(UINT64 newSize);
	[[nodiscard]] std::span<const BYTE> hotSpan() const { return std::span{(const BYTE*)this->pMem, this->sz}; }
	[[nodiscard]] std::span<BYTE> hotSpan() { return std::span{(BYTE*)this->pMem, this->sz}; }
	[[nodiscard]] std::vector<BYTE> readChunk(UINT64 offset, UINT64 numBytes) const;
	[[nodiscard]] std::vector<BYTE> readAll() const { return this->readChunk(0, this->sz); }

private:
	void mapInMemory();
};

}
