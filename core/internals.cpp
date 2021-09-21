
#include <system_error>
#include "internals.h"
#include <commoncontrols.h>
using std::tuple;

int core_internals::Lippincott() noexcept
{
	int code = -1;
	const char* caption = "Oops... unknown error";
	const char* msg = "An unknown exception, not derived from std::exception, was thrown.";

	// https://stackoverflow.com/a/48036486/6923555
	try { throw; }
	catch (const std::invalid_argument& e) { msg = e.what(); caption = "Oops... invalid argument error"; }
	catch (const std::out_of_range& e) { msg = e.what(); caption = "Oops... out of range error"; }
	catch (const std::logic_error& e) { msg = e.what(); caption = "Oops... logic error"; }
	catch (const std::system_error& e) { msg = e.what(); caption = "Oops... system error"; code = e.code().value(); }
	catch (const std::runtime_error& e) { msg = e.what(); caption = "Oops... runtime error"; }
	catch (const std::exception& e) { msg = e.what(); caption = "Oops... generic error"; }

	MessageBoxA(nullptr, msg, caption, MB_ICONERROR);
	return code;
}

HIMAGELIST core_internals::ShellImageList(int shil)
{
	// Implemented here because commoncontrols.h defines an ImageList type.

	HIMAGELIST hil = nullptr;
	if (HRESULT hr = SHGetImageList(shil, IID_IImageList, (void**)(&hil)); FAILED(hr)) {
		throw std::system_error(hr, std::system_category(), "SHGetImageList failed.");
	}
	return hil;
}
