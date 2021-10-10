/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <system_error>
#include <type_traits>
#include <utility>
#include <windows.h>
#include <objbase.h>
#include "base_priv.h"

namespace wl::com {

/// Manages a COM pointer.
/// Created with com::co_create_instance().
///
/// #include <com.h>
template<typename T, typename = std::enable_if_t<std::is_base_of_v<IUnknown, T>>>
class ptr final {
  private:
    T* _ptr = nullptr;

  public:
    /// Destructor. Calls release().
    ~ptr() { this->release(); }

    /// Default constructor.
    ptr() = default;

    /// Move constructor.
    ptr(ptr&& other) noexcept { this->operator=(std::move(other)); }

    /// Allows calling methods directly from the underlying pointer.
    T* operator->() noexcept { return this->_ptr; }

    /// Allows calling methods directly from the underlying pointer.
    const T* operator->() const noexcept { return this->_ptr; }

    operator const T*() const noexcept { return this->_ptr; }
    operator T*() noexcept { return this->_ptr; }

    NODISCARD const T** operator&() const noexcept { return &this->_ptr; }
    NODISCARD T** operator&() noexcept { return &this->_ptr; }

    explicit operator bool() const noexcept { return this->_ptr != nullptr; }

    /// Tells if the underlying pointer is null.
    NODISCARD bool empty() const noexcept { return this->_ptr == nullptr; }

    /// Returns the raw pointer to the COM interface pointer.
    /// @warning Using the raw pointer is dangerous, be careful.
    NODISCARD const T** raw_pptr() const noexcept { return &this->_ptr; }
    /// Returns the raw pointer to the COM interface pointer.
    /// @warning Using the raw pointer is dangerous, be careful.
    NODISCARD T** raw_pptr() noexcept { return &this->_ptr; }

    /// Move assignment operator.
    ptr& operator=(ptr&& other) noexcept {
        this->release();
        std::swap(this->_ptr, other._ptr);
        return *this;
    }

    /// Returns a safe clone of the COM pointer with AddRef().
    /// @see https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-addref
    NODISCARD ptr clone() const noexcept {
        this->_ptr->AddRef();

        ptr clonedObj{};
        clonedObj._ptr = this->_ptr;
        return clonedObj;
    }

    /// Calls IUnknown::QueryInterface() with the given REFIID.
    /// @see https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
    template<typename C, typename = std::enable_if_t<std::is_base_of_v<IUnknown, C>>>
    NODISCARD ptr<C> query_interface(REFIID iid_any) {
        ptr<C> ptrBuf;

        if (HRESULT hr = this->_ptr->QueryInterface(iid_any, reinterpret_cast<void**>(ptrBuf.raw_pptr())); FAILED(hr)) {
            throw std::system_error(hr, std::system_category(), "QueryInterface() failed in " __FUNCTION__ "().");
        }
        return ptrBuf;
    }

    /// Calls IUnknown::QueryInterface() with IID_PPV_ARGS().
    /// @see https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(q)
    template<typename C, typename = std::enable_if_t<std::is_base_of_v<IUnknown, C>>>
    NODISCARD ptr<C> query_interface() {
        ptr<C> ptrBuf;

        if (HRESULT hr = this->_ptr->QueryInterface(IID_PPV_ARGS(ptrBuf.raw_pptr())); FAILED(hr)) {
            throw std::system_error(hr, std::system_category(), "QueryInterface() failed in " __FUNCTION__ "().");
        }
        return ptrBuf;
    }

    /// Calls IUnknown::Release().
    /// @see https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-release
    void release() noexcept {
        if (this->_ptr != nullptr) {
            this->_ptr->Release();
            this->_ptr = nullptr;
        }
    }
};

/// Creates a COM object by calling CoCreateInstance() with the given REFIID.
/// @see https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-cocreateinstance
template<typename T, typename = std::enable_if_t<std::is_base_of_v<IUnknown, T>>>
NODISCARD inline ptr<T> co_create_instance(REFCLSID clsid_any, REFIID iid_any, DWORD clsctxContext = CLSCTX_INPROC_SERVER) {
    ptr<T> ptrBuf;

    if (HRESULT hr = CoCreateInstance(clsid_any, nullptr, clsctxContext, iid_any, reinterpret_cast<LPVOID*>(ptrBuf.raw_pptr())); FAILED(hr)) {
        throw std::system_error(hr, std::system_category(), "CoCreateInstance() failed in " __FUNCTION__ "().");
    }
    return ptrBuf;
}

/// Creates a COM object by calling CoCreateInstance() with IID_PPV_ARGS().
/// @see https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-cocreateinstance
template<typename T, typename = std::enable_if_t<std::is_base_of_v<IUnknown, T>>>
NODISCARD inline ptr<T> co_create_instance(REFCLSID clsid_any, DWORD clsctxContext = CLSCTX_INPROC_SERVER) {
    ptr<T> ptrBuf;

    if (HRESULT hr = CoCreateInstance(clsid_any, nullptr, clsctxContext, IID_PPV_ARGS(ptrBuf.raw_pptr())); FAILED(hr)) {
        throw std::system_error(hr, std::system_category(), "CoCreateInstance() failed in " __FUNCTION__ "().");
    }
    return ptrBuf;
}

// helper macros
#define VAR_BOOL(x) ((x) ? VARIANT_TRUE : VARIANT_FALSE)
#define TO_BOOL(x) ((x == VARIANT_TRUE) ? true : false)
#define CHECK_HRESULT(hr) \
    if (FAILED(hr))       \
        throw std::system_error(hr, std::system_category(), __FILE__ ":" STRINGIFY(__LINE__));

}  // namespace wl::com