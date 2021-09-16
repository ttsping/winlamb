
#include <system_error>
#include "Window.h"
using namespace core;

int Lippincott() noexcept
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
