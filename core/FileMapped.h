
#pragma once
#include <span>
#include <string_view>
#include <vector>
#include "File.h"

namespace core {

// Manages a memory-mapped file.
class FileMapped final {
public:
	enum class Access { READ, RW };

private:
	File file;
	HANDLE hMap;
	void* pMem;
	size_t sz;
	bool readOnly;

public:
	~FileMapped() { this->close(); }

	constexpr FileMapped(FileMapped&& other) noexcept;
	FileMapped& operator=(FileMapped&& other) noexcept;

	FileMapped(std::wstring_view filePath, Access access);

	void close() noexcept;
	[[nodiscard]] constexpr size_t size() const noexcept { return this->sz; }
	void resize(size_t newSize);
	[[nodiscard]] constexpr std::span<const BYTE> hotSpan() const noexcept { return std::span{(const BYTE*)this->pMem, this->sz}; }
	[[nodiscard]] constexpr std::span<BYTE> hotSpan() noexcept { return std::span{(BYTE*)this->pMem, this->sz}; }
	[[nodiscard]] std::vector<BYTE> readChunk(size_t offset, size_t numBytes) const;
	[[nodiscard]] std::vector<BYTE> readAll() const { return this->readChunk(0, this->sz); }

private:
	void mapInMemory();
};

}
