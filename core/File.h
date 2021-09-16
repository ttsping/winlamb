
#pragma once
#include <vector>
#include <Windows.h>

namespace core {

// Encapsulates a file HANDLE.
class File final {
private:
	HANDLE hf = nullptr;

public:
	enum class Access { READ_EXISTING, RW_EXISTING, RW_OPEN_OR_CREATE };

	~File() { this->close(); }

	File() = default;
	File(const wchar_t* filePath, Access access) { this->open(filePath, access); }
	File(File&& other) noexcept;
	File& operator=(File&& other) noexcept;

	void close() noexcept;
	DWORD open(const wchar_t* filePath, Access access);
	HANDLE handle() const noexcept { return this->hf; }
	INT64 offsetPtr() const;
	void offsetPtrRewind() const;
	UINT64 size() const;
	DWORD write(const std::vector<BYTE>& data) const;
	std::vector<BYTE> readAll() const;
};

}
