/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <minwindef.h>

namespace wl {
namespace detail {
template<HANDLE InvalidHandleType>
class auto_handle_t {
  public:
    static constexpr HANDLE kInvalidHandleValue = InvalidHandleType;
    auto_handle_t() = default;
    auto_handle_t(HANDLE h) : handle_(h) {}
    ~auto_handle_t() { release(); }
    auto_handle_t(auto_handle_t&& rhs) {
        handle_ = rhs.handle_;
        rhs.handle_ = kInvalidHandleValue;
    }
    auto_handle_t& operator=(auto_handle_t&& rhs) {
        handle_ = rhs.handle_;
        rhs.handle_ = kInvalidHandleValue;
        return *this;
    }
    auto_handle_t(const auto_handle_t&) = delete;
    auto_handle_t& operator=(const auto_handle_t&) = delete;
    operator HANDLE() const { return handle_; }
    PHANDLE operator&() { return &handle_; }
    operator bool() const { return is_valid(); }

    bool is_valid() const { return handle_ != kInvalidHandleValue; }

    void release() {
        if (is_valid()) {
            CloseHandle(handle_);
            handle_ = kInvalidHandleValue;
        }
    }

    HANDLE detach() {
        HANDLE h = handle_;
        handle_ = kInvalidHandleValue;
        return h;
    }

  protected:
    HANDLE handle_{ kInvalidHandleValue };
};
}  // namespace detail

using auto_handle = detail::auto_handle_t<nullptr>;
using auto_handle2 = detail::auto_handle_t<INVALID_HANDLE_VALUE>;

}  // namespace wl