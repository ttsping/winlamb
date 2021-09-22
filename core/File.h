
#pragma once
#include <span>
#include <string_view>
#include <vector>
#include <Windows.h>

namespace core {

// Owning wrapper to a file HANDLE.
class File final {
public:
	enum class Access { READ_EXISTING, RW_EXISTING, RW_OPEN_OR_CREATE };

	// Manages a file lock.
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
	HANDLE hf;

public:
	~File() { this->close(); }

	constexpr File(File&& other) noexcept : hf{nullptr} { std::swap(this->hf, other.hf); }
	File& operator=(File&& other) noexcept;

	File(std::wstring_view filePath, Access access);

	void close() noexcept;
	[[nodiscard]] constexpr HANDLE handle() const noexcept { return this->hf; }
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
