/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <system_error>
#include <crtdbg.h>
#include <Windows.h>
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

namespace _wli::run {

enum class post_quit_on_catch { YES, NO };

// Executes a lambda and catches any exception, showing a MessageBox().
template<typename F>
inline void catch_all_exceptions(F&& func, post_quit_on_catch catchAction) noexcept
{
	// Intended to handle the exceptions of user lambdas.
	// https://stackoverflow.com/a/18792495

	auto honk = [](const char* title, const char* text) {
		MessageBoxA(nullptr, text, title, MB_ICONERROR);
		OutputDebugStringA("---\n");
		OutputDebugStringA(title);
		OutputDebugStringA("\n");
		OutputDebugStringA(text);
		OutputDebugStringA("\n---\n");
	};

	try {

		try {
			func();
		} catch (const std::system_error& e) {
			char buf[2048]{}; // arbitrary length
			wsprintfA(buf, "[%d 0x%02x] %s",
				e.code().value(), e.code().value(), e.what());
			honk("System exception", buf);
			throw;
		} catch (const std::runtime_error& e) {
			honk("Runtime exception", e.what());
			throw;
		} catch (const std::invalid_argument& e) {
			honk("Invalid argument exception", e.what());
			throw;
		} catch (const std::logic_error& e) {
			honk("Logic exception", e.what());
			throw;
		} catch (const std::bad_optional_access& e) {
			honk("Bad optional access exception", e.what());
			throw;
		} catch (const std::exception& e) {
			honk("Exception", e.what());
			MessageBoxA(nullptr, e.what(), "Exception", MB_ICONERROR);
			throw;
		} catch (...) {
			honk("Unknown exception", "An unknown exception was thrown.");
			throw;
		}

	} catch (...) {
		if (catchAction == post_quit_on_catch::YES) {
			PostQuitMessage(-1);
		}
	}
}

// Instantiates a class and calls run_as_main(), catching any exception.
template<typename wnd_mainT>
int catch_run_main(HINSTANCE hInst, int cmdShow) noexcept
{
	struct LeakDetect {
		~LeakDetect() { _ASSERT(!_CrtDumpMemoryLeaks()); }
	};
	static LeakDetect leakDetect; // destructor will run after all static destructors

	int ret = 0;
	catch_all_exceptions([&ret, hInst, cmdShow]() {
		wnd_mainT w; // instantiate main window object
		ret = w.run_as_main(hInst, cmdShow);
	}, post_quit_on_catch::NO);
	return ret;
}

}//namespace _wli::run

// Instantiates a main class into a generic WinMain function.
#define RUN(wnd_mainT) \
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int cmdShow) { \
	return _wli::run::catch_run_main<wnd_mainT>(hInst, cmdShow); \
}