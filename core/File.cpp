
#include <system_error>
#include <utility>
#include "File.h"
#include "internals.h"
using namespace core;
using std::span;
using std::system_error;
using std::vector;
using std::wstring_view;

File::Lock::Lock(const File& file, size_t offset, size_t numBytes)
	: file{file}, off{offset}, sz{numBytes}
{
	if (!LockFile(this->file.handle(),
		core_internals::Lo64(this->off), core_internals::Hi64(this->off),
		core_internals::Lo64(this->sz), core_internals::Hi64(this->sz)))
	{
		throw system_error(GetLastError(), std::system_category(), "LockFile failed");
	}
}

void File::Lock::unlock()
{
	if (this->off && this->sz) {
		UnlockFile(this->file.handle(),
			core_internals::Lo64(this->off), core_internals::Hi64(this->off),
			core_internals::Lo64(this->sz), core_internals::Hi64(this->sz));
		this->off = this->sz = 0;
	}
}

File& File::operator=(File&& other)
{
	this->close();
	std::swap(this->hf, other.hf);
	return *this;
}

File::File(wstring_view filePath, Access access)
	: hf{nullptr}
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

	if (!(this->hf = CreateFile(filePath.data(), readWrite, share, nullptr,
		disposition, FILE_ATTRIBUTE_NORMAL, nullptr)))
	{
		throw system_error(GetLastError(), std::system_category(), "CreateFile failed");
	}
}

void File::close()
{
	if (this->hf) {
		CloseHandle(this->hf);
		this->hf = nullptr;
	}
}

void File::eraseAndWrite(span<const BYTE> bytes) const
{
	this->resize(bytes.size());
	this->write(bytes);
	this->offsetPtrRewind();
}

size_t File::offsetPtr() const
{
	size_t offset = (size_t)SetFilePointerEx(this->hf, {0}, nullptr, FILE_CURRENT);
	if (!offset) {
		if (DWORD err = GetLastError(); err != ERROR_SUCCESS) {
			throw system_error(GetLastError(), std::system_category(), "SetFilePointerEx failed");
		}
	}
	return offset;
}

void File::offsetPtrRewind() const
{
	if (!SetFilePointerEx(this->hf, {0}, nullptr, FILE_BEGIN)) {
		if (DWORD err = GetLastError(); err != ERROR_SUCCESS) {
			throw system_error(GetLastError(), std::system_category(), "SetFilePointerEx failed");
		}
	}
}

vector<BYTE> File::readAll() const
{
	this->offsetPtrRewind();
	size_t len = this->size();
	vector<BYTE> buf(len, 0x00); // alloc buffer
	DWORD numRead = 0;

	if (!ReadFile(this->hf, &buf[0], (DWORD)len, &numRead, nullptr)) {
		throw system_error(GetLastError(), std::system_category(), "ReadFile failed");
	}
	this->offsetPtrRewind();
	return buf;
}

void File::resize(size_t newSize) const
{
	LARGE_INTEGER li = {0};
	li.QuadPart = newSize;

	if (!SetFilePointerEx(this->hf, li, nullptr, FILE_BEGIN)) {
		if (DWORD err = GetLastError(); err != ERROR_SUCCESS) {
			throw system_error(GetLastError(), std::system_category(), "SetFilePointerEx failed");
		}
	}

	if (!SetEndOfFile(this->hf)) {
		throw system_error(GetLastError(), std::system_category(), "SetEndOfFile failed");
	}

	this->offsetPtrRewind();
}

size_t File::size() const
{
	LARGE_INTEGER li = {0};
	if (!GetFileSizeEx(this->hf, &li)) {
		throw system_error(GetLastError(), std::system_category(), "GetFileSizeEx failed");
	}
	return (size_t)li.QuadPart;
}

void File::write(span<const BYTE> bytes) const
{
	DWORD written = 0;
	if (!WriteFile(this->hf, bytes.data(), (DWORD)bytes.size(), &written, nullptr)) {
		throw system_error(GetLastError(), std::system_category(), "WriteFile failed");
	}
}
