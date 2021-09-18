
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
		UINT64 off, sz;
		Lock(const File& file, UINT64 offset, UINT64 numBytes);
	public:
		~Lock() { this->unlock(); }
		void unlock() const noexcept;
		[[nodiscard]] constexpr UINT64 offset() const { return this->off; }
		[[nodiscard]] constexpr UINT64 numBytes() const { return this->sz; }
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
	void open(std::wstring_view filePath, Access access);
	[[nodiscard]] Lock lock(UINT64 offset, UINT64 numBytes) const { return Lock(*this, offset, numBytes); }
	[[nodiscard]] constexpr HANDLE handle() const { return this->hf; }
	[[nodiscard]] INT64 offsetPtr() const;
	void offsetPtrRewind() const;
	[[nodiscard]] UINT64 size() const;
	void resize(UINT64 newSize) const;
	[[nodiscard]] std::vector<BYTE> readAll() const;
	void write(std::span<const BYTE> bytes) const;
	void eraseAndWrite(std::span<const BYTE> bytes) const;
};

}
