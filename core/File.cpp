
#include <system_error>
#include <utility>
#include "File.h"
#include "internals.h"
using namespace core;
using std::vector;
using std::wstring_view;

File::File(File&& other) noexcept
	: hf{other.hf}
{
	other.hf = nullptr;
}

File& File::operator=(File&& other) noexcept
{
	this->close();
	std::swap(this->hf, other.hf);
	return *this;
}

void File::close() noexcept
{
	if (this->hf) {
		CloseHandle(this->hf);
		this->hf = nullptr;
	}
}

void File::open(wstring_view filePath, Access access)
{
	DWORD readWrite = GENERIC_READ | (access == Access::READ_EXISTING ? 0 : GENERIC_WRITE);
	DWORD share = access == Access::READ_EXISTING ? FILE_SHARE_READ : 0;
	DWORD disposition = 0;

	switch (access) {
	case Access::READ_EXISTING:
		disposition = OPEN_EXISTING;
		break;
	case Access::RW_EXISTING:
		disposition = OPEN_EXISTING;
		break;
	case Access::RW_OPEN_OR_CREATE:
		disposition = OPEN_ALWAYS;
	}

	if (!(this->hf = CreateFileW(filePath.data(), readWrite, share, nullptr,
		disposition, FILE_ATTRIBUTE_NORMAL, nullptr)))
	{
		throw std::system_error(GetLastError(), std::system_category(), "CreateFileW failed");
	}
}

INT64 File::offsetPtr() const
{
	INT64 offset = (INT64)SetFilePointerEx(this->hf, {0}, nullptr, FILE_CURRENT);
	if (!offset) {
		if (DWORD err = GetLastError(); err != ERROR_SUCCESS) {
			throw std::system_error(GetLastError(), std::system_category(), "SetFilePointerEx failed");
		}
	}
	return offset;
}

void File::offsetPtrRewind() const
{
	if (!SetFilePointerEx(this->hf, {0}, nullptr, FILE_BEGIN)) {
		if (DWORD err = GetLastError(); err != ERROR_SUCCESS) {
			throw std::system_error(GetLastError(), std::system_category(), "SetFilePointerEx failed");
		}
	}
}

UINT64 File::size() const
{
	LARGE_INTEGER li = {0};
	if (!GetFileSizeEx(this->hf, &li)) {
		throw std::system_error(GetLastError(), std::system_category(), "GetFileSizeEx failed");
	}
	return (UINT64)li.QuadPart;
}

void File::resize(UINT64 newSize) const
{
	LARGE_INTEGER li = {0};
	li.QuadPart = newSize;

	if (!SetFilePointerEx(this->hf, li, nullptr, FILE_BEGIN)) {
		if (DWORD err = GetLastError(); err != ERROR_SUCCESS) {
			throw std::system_error(GetLastError(), std::system_category(), "SetFilePointerEx failed");
		}
	}

	if (!SetEndOfFile(this->hf)) {
		throw std::system_error(GetLastError(), std::system_category(), "SetEndOfFile failed");
	}

	this->offsetPtrRewind();
}

vector<BYTE> File::readAll() const
{
	this->offsetPtrRewind();
	UINT64 len = this->size();
	vector<BYTE> buf(len, 0x00); // alloc buffer
	DWORD numRead = 0;

	if (!ReadFile(this->hf, &buf[0], (DWORD)len, &numRead, nullptr)) {
		throw std::system_error(GetLastError(), std::system_category(), "ReadFile failed");
	}
	this->offsetPtrRewind();
	return buf;
}

void File::write(const vector<BYTE>& data) const
{
	DWORD written = 0;
	if (!WriteFile(this->hf, &data[0], (DWORD)data.size(), &written, nullptr)) {
		throw std::system_error(GetLastError(), std::system_category(), "WriteFile failed");
	}
}

void File::eraseAndWrite(const vector<BYTE>& data) const
{
	this->resize(data.size());
	this->write(data);
	this->offsetPtrRewind();
}

File::Lock::Lock(const File& file, UINT64 offset, UINT64 numBytes)
	: file{file}, offsetL{offset}, numBytesL{numBytes}
{
	if (!LockFile(this->file.handle(),
		core_internals::Lo64(this->offsetL), core_internals::Hi64(this->offsetL),
		core_internals::Lo64(this->numBytesL), core_internals::Hi64(this->numBytesL)))
	{
		throw std::system_error(GetLastError(), std::system_category(), "LockFile failed");
	}
}

void File::Lock::unlock() const noexcept
{
	UnlockFile(this->file.handle(),
		core_internals::Lo64(this->offsetL), core_internals::Hi64(this->offsetL),
		core_internals::Lo64(this->numBytesL), core_internals::Hi64(this->numBytesL));
}
