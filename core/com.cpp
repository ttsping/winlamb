
#include <system_error>
#include "com.h"
#include <Windows.h>
#include <objbase.h>
using namespace core;
using std::system_error;

void core_internals::ThrowSysErr(HRESULT hr, const char* msg)
{
	// Implemented here to avoid #include <system_error> in header.
	throw system_error(hr, std::system_category(), msg);
}

ComLibrary::~ComLibrary()
{
	CoUninitialize();
}

ComLibrary::ComLibrary()
{
	if (HRESULT hr = CoInitialize(nullptr); FAILED(hr)) {
		throw system_error(hr, std::system_category(), "CoInitialize failed");
	}
}
