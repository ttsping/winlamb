
#include <utility>
#include "File.h"
using namespace core;
using std::vector;

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

DWORD File::open(const wchar_t* filePath, Access access)
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

	return (this->hf = CreateFileW(filePath, readWrite, share, nullptr,
		disposition, FILE_ATTRIBUTE_NORMAL, nullptr))
		? ERROR_SUCCESS : GetLastError();
}

INT64 File::offsetPtr() const
{
	return (INT64)SetFilePointerEx(this->hf, {0}, nullptr, FILE_CURRENT);
}

void File::offsetPtrRewind() const
{
	SetFilePointerEx(this->hf, {0}, nullptr, FILE_BEGIN);
}

UINT64 File::size() const
{
	LARGE_INTEGER li = {0};
	GetFileSizeEx(this->hf, &li);
	return (UINT64)li.QuadPart;
}

DWORD File::write(const vector<BYTE>& data) const
{
	DWORD written = 0;
	if (!WriteFile(this->hf, &data[0], (DWORD)data.size(), &written, nullptr)) {
		return GetLastError();
	}
	return ERROR_SUCCESS;
}

vector<BYTE> File::readAll() const
{
	this->offsetPtrRewind();
	UINT64 len = this->size();
	vector<BYTE> buf(len, 0x00);
	DWORD numRead = 0;

	if (!ReadFile(this->hf, &buf[0], (DWORD)len, &numRead, nullptr)) {

	}
	this->offsetPtrRewind();
	return buf;
}
