
#pragma once
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
		UINT64 offsetL, numBytesL;
		Lock(const File& file, UINT64 offset, UINT64 numBytes);
	public:
		~Lock() { this->unlock(); }
		void unlock() const noexcept;
	};

private:
	HANDLE hf = nullptr;

public:
	enum class Access { READ_EXISTING, RW_EXISTING, RW_OPEN_OR_CREATE };

	~File() { this->close(); }

	File() = default;
	File(std::wstring_view filePath, Access access) { this->open(filePath, access); }
	File(File&& other) noexcept;
	File& operator=(File&& other) noexcept;

	void close() noexcept;
	void open(std::wstring_view filePath, Access access);
	[[nodiscard]] Lock lock(UINT64 offset, UINT64 numBytes) const { return Lock(*this, offset, numBytes); }
	[[nodiscard]] HANDLE handle() const noexcept { return this->hf; }
	[[nodiscard]] INT64 offsetPtr() const;
	void offsetPtrRewind() const;
	[[nodiscard]] UINT64 size() const;
	void resize(UINT64 newSize) const;
	[[nodiscard]] std::vector<BYTE> readAll() const;
	void write(const std::vector<BYTE>& data) const;
	void eraseAndWrite(const std::vector<BYTE>& data) const;
	
};

}
