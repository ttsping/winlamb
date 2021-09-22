
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
		[[nodiscard]] constexpr size_t numBytes() const { return this->sz; }
		[[nodiscard]] constexpr size_t offset() const { return this->off; }
		void unlock();
	};

private:
	HANDLE hf;

public:
	~File() { this->close(); }

	constexpr File(File&& other) : hf{nullptr} { std::swap(this->hf, other.hf); }
	File& operator=(File&& other);

	File(std::wstring_view filePath, Access access);

	void close();
	void eraseAndWrite(std::span<const BYTE> bytes) const;
	[[nodiscard]] constexpr HANDLE handle() const { return this->hf; }
	[[nodiscard]] Lock lock(size_t offset, size_t numBytes) const { return Lock(*this, offset, numBytes); }
	[[nodiscard]] size_t offsetPtr() const;
	void offsetPtrRewind() const;
	[[nodiscard]] std::vector<BYTE> readAll() const;
	void resize(size_t newSize) const;
	[[nodiscard]] size_t size() const;
	void write(std::span<const BYTE> bytes) const;
};

}
