
#include <system_error>
#include "internals.h"
#include "str.h"
#include <CommCtrl.h>
using std::tuple;
using std::wstring;

int core_internals::Lippincott() noexcept
{
	using core::str::ToWide;

	int code = -1;
	const wchar_t* instr = L"Unknown exception";
	wstring msg = L"An unknown exception, not derived from std::exception, was thrown.";

	// https://stackoverflow.com/a/48036486/6923555
	try { throw; }
	catch (const std::invalid_argument& e) { msg = ToWide(e.what()); instr = L"Invalid argument exception"; }
	catch (const std::out_of_range& e) { msg = ToWide(e.what()); instr = L"Out of range exception"; }
	catch (const std::logic_error& e) { msg = ToWide(e.what()); instr = L"Logic exception"; }
	catch (const std::system_error& e) { msg = ToWide(e.what()); instr = L"System exception"; code = e.code().value(); }
	catch (const std::runtime_error& e) { msg = ToWide(e.what()); instr = L"Runtime exception"; }
	catch (const std::exception& e) { msg = ToWide(e.what()); instr = L"Generic exception"; }

	TASKDIALOG_BUTTON btn = {0};
	btn.nButtonID = IDOK;
	btn.pszButtonText = L"Terminate";

	TASKDIALOGCONFIG tdc = {0};
	tdc.cbSize = sizeof(TASKDIALOGCONFIG);
	tdc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
	tdc.cButtons = 1;
	tdc.pButtons = &btn;
	tdc.pszMainIcon = TD_ERROR_ICON;
	tdc.pszWindowTitle = L"Oops...";
	tdc.pszMainInstruction = instr;
	tdc.pszContent = msg.c_str();

	TaskDialogIndirect(&tdc, nullptr, nullptr, nullptr);

	return code;
}
