
#pragma once
#include <span>
#include <string_view>
#include <vector>
#include "File.h"

namespace core {

// Manages a memory-mapped file.
class FileMapped final {
private:
	File file;
	HANDLE hMap = nullptr;
	void* pMem = nullptr;
	size_t sz = 0;
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
	[[nodiscard]] constexpr size_t size() const { return this->sz; }
	void resize(size_t newSize);
	[[nodiscard]] constexpr std::span<const BYTE> hotSpan() const { return std::span{(const BYTE*)this->pMem, this->sz}; }
	[[nodiscard]] constexpr std::span<BYTE> hotSpan() { return std::span{(BYTE*)this->pMem, this->sz}; }
	[[nodiscard]] std::vector<BYTE> readChunk(size_t offset, size_t numBytes) const;
	[[nodiscard]] std::vector<BYTE> readAll() const { return this->readChunk(0, this->sz); }

private:
	void mapInMemory();
};

}
