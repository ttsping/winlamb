
#pragma once
#include <span>
#include <string_view>
#include <vector>
#include <Windows.h>

namespace core {

// Encapsulates a file HANDLE.
class File final {
public:
	// Encapsulates a file lock.
	class Lock final {
		friend File;
	private:
		const File& file;
		size_t off, sz;
		Lock(const File& file, size_t offset, size_t numBytes);
	public:
		~Lock() { this->unlock(); }
		void unlock() noexcept;
		[[nodiscard]] constexpr size_t offset() const { return this->off; }
		[[nodiscard]] constexpr size_t numBytes() const { return this->sz; }
	};

private:
	HANDLE hf = nullptr;

public:
	enum class Access { READ_EXISTING, RW_EXISTING, RW_OPEN_OR_CREATE };

	~File() { this->close(); }

	File() = default;
	File(std::wstring_view filePath, Access access) { this->open(filePath, access); }
	File(File&& other) noexcept { this->operator=(std::move(other)); }
	File& operator=(File&& other) noexcept;

	void close() noexcept;
	[[nodiscard]] constexpr HANDLE handle() const { return this->hf; }
	void open(std::wstring_view filePath, Access access);
	[[nodiscard]] Lock lock(size_t offset, size_t numBytes) const { return Lock(*this, offset, numBytes); }
	[[nodiscard]] INT64 offsetPtr() const;
	void offsetPtrRewind() const;
	[[nodiscard]] size_t size() const;
	void resize(size_t newSize) const;
	[[nodiscard]] std::vector<BYTE> readAll() const;
	void write(std::span<const BYTE> bytes) const;
	void eraseAndWrite(std::span<const BYTE> bytes) const;
};

}
