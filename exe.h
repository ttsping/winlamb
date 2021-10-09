/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <array>
#include <optional>
#include <string_view>
#include <system_error>
#include <string>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include "internal/base_priv.h"
#include "exception.h"
#include "com.h"
#include "str.h"
#include "handle.h"
#pragma comment(lib, "version.lib")
#pragma comment(lib, "shlwapi.lib")

/// Executable/DLL related utilities.
///
/// #include <exe.h>
namespace wl::exe {

struct execution_options {
    std::wstring app_path;
    std::wstring parameters;
    std::wstring working_dir;
    uint16_t show_cmd = SW_SHOWNORMAL;
    uint32_t creation_flags = 0;
    bool exec_sync = false;
    HANDLE exec_token = nullptr;
};

/// Retrieves the program's command line, with the tokens parsed.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-getcommandlinew
NODISCARD FORCEINLINE std::vector<std::wstring> command_line() { return str::split_quoted(GetCommandLineW()); }

/// <summary>
///  Generate command line which can be used in CreateProcess* APIs;
/// </summary>
/// <param name="appName">executable path</param>
/// <param name="cmdLine">execute parameters</param>
/// <returns>command line</returns>
inline std::wstring concat_command_line(const std::wstring& appName, const std::wstring& cmdLine) {
    std::wstring res = appName;
    if (!res.empty()) {
        res.resize(MAX_PATH * 3);
        PathQuoteSpacesW(res.data());
        str::trim_nulls(res);
        res += L" ";
    }
    res += cmdLine;
    return res;
}

/// Retrieves path to current running EXE itself.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew
NODISCARD FORCEINLINE std::wstring own_path() {
    wchar_t buf[MAX_PATH + 1]{};
    if (GetModuleFileNameW(nullptr, buf, ARRAYSIZE(buf)) == 0) {  // full path name
        THROW_API_EXCEPTION("GetModuleFileName");
    }
    std::wstring ret{ buf };
    ret.resize(ret.find_last_of(L'\\'));  // truncate removing EXE filename and trailing backslash

#if defined(_MSC_VER) && defined(_DEBUG)
    ret.resize(ret.find_last_of(L'\\'));  // bypass "Debug" folder, remove trailing backslash too
#ifdef _WIN64
    ret.resize(ret.find_last_of(L'\\'));  // bypass "x64" folder, remove trailing backslash again
#endif
#endif

    return ret;
}

/// <summary>
/// Check if current process is running as administrator
/// </summary>
/// <param name="">none</param>
/// <returns>return true if own process is running as administrator, otherwise false.</returns>
inline bool is_run_as_admin(void) noexcept {
    char admin_group[SECURITY_MAX_SID_SIZE] = { 0 };
    DWORD cb = sizeof(admin_group);
    if (CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &admin_group, &cb)) {
        BOOL is_admin = FALSE;
        return CheckTokenMembership(NULL, admin_group, &is_admin) && !!is_admin;
    }
    return false;
}

/// <summary>
/// Get the token of shell process
/// </summary>
/// <param name="">none</param>
/// <returns>return the token if successfully.</returns>
HANDLE get_shell_process_token(void) {
    if (!is_run_as_admin()) {
        throw std::runtime_error("must be called with admin privilege");
    }
    // https://web.archive.org/web/20091231071322/http://blogs.msdn.com/aaron_margosis/archive/2009/06/06/faq-how-do-i-start-a-program-as-the-desktop-user-from-an-elevated-app.aspx
    // step 1: Enable the SeIncreaseQuotaPrivilege in current token
    wl::auto_handle proc_token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &proc_token)) {
        THROW_API_EXCEPTION("OpenProcessToken");
    }

    TOKEN_PRIVILEGES tkp;
    tkp.PrivilegeCount = 1;
    LookupPrivilegeValueW(NULL, SE_INCREASE_QUOTA_NAME, &tkp.Privileges[0].Luid);
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(proc_token, FALSE, &tkp, 0, NULL, NULL);
    DWORD last_err = GetLastError();
    if (ERROR_SUCCESS != last_err) {
        THROW_API_EXCEPTION("AdjustTokenPrivileges");
    }

    // step 2: Get an HWND representing the desktop shell and get the process ID (PID) of the process associated with the window
    HWND shell_wnd = GetShellWindow();
    if (shell_wnd == nullptr) {
        THROW_API_EXCEPTION("GetShellWindow");
    }
    DWORD shell_pid;
    GetWindowThreadProcessId(shell_wnd, &shell_pid);
    if (shell_pid == 0) {
        THROW_API_EXCEPTION("GetWindowThreadProcessId");
    }

    // step 3: Get the access token from the shell process and duplicate
    wl::auto_handle shell_proc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, shell_pid);
    if (!shell_proc) {
        THROW_API_EXCEPTION("OpenProcess");
    }

    wl::auto_handle shell_token = nullptr;
    HANDLE primary_token = nullptr;
    // Get the process token of the desktop shell.
    if (!OpenProcessToken(shell_proc, TOKEN_DUPLICATE, &shell_token)) {
        THROW_API_EXCEPTION("OpenProcessToken");
    }

    // Duplicate the shell's process token to get a primary token.
    const DWORD token_rights = TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID;
    if (!DuplicateTokenEx(shell_token, token_rights, NULL, SecurityImpersonation, TokenPrimary, &primary_token)) {
        THROW_API_EXCEPTION("DuplicateTokenEx");
    }
    return primary_token;
}

/// Reads the version information from an EXE or DLL.
NODISCARD std::optional<std::array<UINT, 4>> read_version(std::wstring_view exeOrDllPath) {
    DWORD szVer = GetFileVersionInfoSizeW(exeOrDllPath.data(), nullptr);
    if (szVer == 0) {
        THROW_API_EXCEPTION("GetFileVersionInfoSize");
    }

    std::vector<wchar_t> infoBlock(szVer, L'\0');
    if (GetFileVersionInfoW(exeOrDllPath.data(), 0, szVer, &infoBlock[0]) == 0) {
        THROW_API_EXCEPTION("GetFileVersionInfo");
    }

    BYTE* lpBuf = nullptr;
    UINT blockSize = 0;
    if (VerQueryValueW(&infoBlock[0], L"\\", reinterpret_cast<void**>(&lpBuf), &blockSize) == FALSE || blockSize == 0) {
        return std::nullopt;  // no information available, not an error
    }

    VS_FIXEDFILEINFO* verInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(lpBuf);
    return { { (verInfo->dwFileVersionMS >> 16) & 0xffff, (verInfo->dwFileVersionMS >> 0) & 0xffff, (verInfo->dwFileVersionLS >> 16) & 0xffff,
               (verInfo->dwFileVersionLS >> 0) & 0xffff } };
}

/// Reads version information of current running EXE itself.
NODISCARD FORCEINLINE std::optional<std::array<UINT, 4>> read_own_version() {
    wchar_t buf[MAX_PATH + 1]{};
    if (GetModuleFileNameW(nullptr, buf, ARRAYSIZE(buf)) == 0) {  // full path name
        THROW_API_EXCEPTION("GetModuleFileName");
    }
    return read_version(buf);
}

/// <summary>
/// Wrapper of CreateProcess with more options
/// </summary>
/// <param name="options">Exection options</param>
/// <returns>return TRUE/FALSE in async mode, or exit mode in sync mode</returns>
DWORD run_process_ex(const execution_options& options) {
    DWORD res = 0;
    STARTUPINFO si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = options.show_cmd;
    PROCESS_INFORMATION pi{};
    auto exec_cmd = concat_command_line(options.app_path, options.parameters);
    const wchar_t* working_dir = options.working_dir.empty() ? nullptr : options.working_dir.c_str();
    if (options.exec_token != nullptr) {
        if (!CreateProcessWithTokenW(options.exec_token, 0, nullptr, std::data(exec_cmd), options.creation_flags, nullptr, working_dir, &si, &pi)) {
            THROW_API_EXCEPTION("CreateProcessWithToken");
        }
    } else {
        if (!CreateProcessW(nullptr, std::data(exec_cmd), nullptr, nullptr, FALSE, options.creation_flags, nullptr, working_dir, &si, &pi))
            THROW_API_EXCEPTION("CreateProcess");
    }
    if (options.exec_sync) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &res);
    } else {
        res = TRUE;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return res;
}

/// Asynchronous execution of a command line, in a separated process.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw
FORCEINLINE DWORD run_process(std::wstring_view cmdLine) {
    execution_options options;
    options.app_path = cmdLine.data();
    options.exec_sync = false;
    return run_process_ex(options);
}

/// <summary>
/// Synchronous execution of a command line, in a separated process.
/// </summary>
/// <param name="cmdLine">Command line</param>
/// <returns>return true if executed successfully.</returns>
FORCEINLINE bool run_process_sync(std::wstring_view cmdLine) {
    execution_options options;
    options.app_path = cmdLine.data();
    options.exec_sync = true;
    return !!run_process_ex(options);
}

/// Runs the shell-associated program to the given file
/// Ex.: for a TXT file, will run Notepad.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutew
FORCEINLINE void run_associated_shell(std::wstring_view fileToBeOpened, INT showCmd = SW_SHOWNORMAL) {
    com::lib comLib{ com::lib::init::NOW };
    int h = static_cast<int>(reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", fileToBeOpened.data(), nullptr, nullptr, showCmd)));
    if (h <= 8) {
        throw std::system_error(h, std::system_category(), "ShellExecute failed.");
    } else if (h <= 32) {
        throw std::runtime_error(str::unicode_to_ansi(str::format(L"ShellExecute failed: error %d in " __FUNCTION__ "().", h)));
    }
}

/// <summary>
/// Wrapper of ShellExecute
/// </summary>
/// <param name="op">Verb</param>
/// <param name="file">File</param>
/// <param name="parameters">Additional parameters</param>
/// <param name="working_dir">Working directory</param>
/// <param name="show_cmd">Windows show flags</param>
FORCEINLINE void shell_exec_ex(std::wstring_view op, std::wstring_view file, const wchar_t* parameters = nullptr, const wchar_t* working_dir = nullptr,
    int show_cmd = SW_SHOWNORMAL) {
    com::lib comLib{ com::lib::init::NOW };
    int res = static_cast<int>(reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, std::data(op), std::data(file), parameters, working_dir, show_cmd)));
    if (res <= 8) {
        throw api_exception("ShellExecute", res);
    } else if (res <= 32) {
        throw std::runtime_error(str::format("ShellExecute failed: error %d.", res));
    }
}

/// <summary>
/// Run process as desktop user
/// </summary>
/// <param name="options"></param>
/// <returns></returns>
FORCEINLINE DWORD run_process_as_standard_user(const execution_options& options) {
    DWORD res = 0;
    if (!is_run_as_admin()) {
        return run_process_ex(options);
    }
    execution_options exec_options{ options };
    if (exec_options.exec_token == nullptr) {
        auto_handle h{ exec_options.exec_token = get_shell_process_token() };
        res = run_process_ex(exec_options);
    }
    return res;
}

/// <summary>
/// Run process as desktop user
/// </summary>
/// <param name="options"></param>
/// <returns></returns>
FORCEINLINE DWORD run_process_as_standard_user(std::wstring_view cmdLine) {
    execution_options options;
    options.app_path = cmdLine;
    return run_process_as_standard_user(options);
}

/// <summary>
/// Run process as administator, Note if current process is not run as admin, then the created process run in async mode
/// (fixme: to use ShellExecuteEx instead).
/// </summary>
/// <param name="options"></param>
/// <returns></returns>
FORCEINLINE DWORD run_process_as_admin(const execution_options& options) {
    if (is_run_as_admin()) {
        return run_process_ex(options);
    }
    try {
        const wchar_t* parameters = options.parameters.empty() ? nullptr : options.parameters.c_str();
        const wchar_t* working_dir = options.working_dir.empty() ? nullptr : options.working_dir.c_str();
        shell_exec_ex(L"runas", options.app_path, parameters, working_dir, options.show_cmd);
    } catch (std::exception&) {
        return FALSE;
    }
    return TRUE;
}

/// <summary>
/// Run process as administator, Note if current process is not run as admin, then the created process run in async mode
/// (fixme: to use ShellExecuteEx instead).
/// </summary>
/// <param name="options"></param>
/// <returns></returns>
FORCEINLINE DWORD run_process_as_admin(std::wstring_view cmdLine) {
    execution_options options;
    options.app_path = cmdLine;
    return run_process_as_admin(options);
}

}  // namespace wl::exe