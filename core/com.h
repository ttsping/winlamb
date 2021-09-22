
#pragma once
#include <type_traits>
#include <objbase.h>

namespace core_internals { void ThrowSysErr(HRESULT hr, const char* msg); }

namespace core {

// Takes care of loading/unloading the COM library.
class ComLibrary final {
public:
	~ComLibrary();
	ComLibrary();
};

// Concept to restrict IUnknown-derived classes.
template<typename T>
concept IUnknownDerived = std::is_base_of_v<IUnknown, T>;

// Owning wrapper to a COM pointer.
template<IUnknownDerived T>
class ComPtr final {
private:
	T* ptr;

public:
	~ComPtr() { this->release(); }

	constexpr ComPtr(ComPtr&& other) noexcept : ptr{nullptr} { std::swap(this->ptr, other.ptr); }
	ComPtr& operator=(ComPtr&& other) noexcept;

	constexpr ComPtr() noexcept : ptr{nullptr} { }
	explicit ComPtr(REFCLSID clsid_something, DWORD clsContext = CLSCTX_INPROC_SERVER);
	ComPtr(REFCLSID clsid_something, REFIID iid_something, DWORD clsContext = CLSCTX_INPROC_SERVER);

	void release() noexcept;
	constexpr operator const T*() const noexcept { return this->ptr; }
	constexpr operator T*() noexcept { return this->ptr; }
	constexpr const T* operator->() const noexcept { return this->ptr; }
	constexpr T* operator->() noexcept { return this->ptr; }
	constexpr const T** operator&() const noexcept { return &this->ptr; }
	constexpr T** operator&() noexcept { return &this->ptr; }

	template<IUnknownDerived Q> ComPtr<Q> queryInterface();
	template<IUnknownDerived Q> ComPtr<Q> queryInterface(REFIID iid_something);
};

template<IUnknownDerived T>
ComPtr<T>::ComPtr(REFCLSID clsid_something, DWORD clsContext)
	: ptr{nullptr}
{
	if (HRESULT hr = CoCreateInstance(clsid_something, nullptr, clsContext, IID_PPV_ARGS(&this->ptr));
		FAILED(hr)) {
		core_internals::ThrowSysErr(hr, "CoCreateInstance failed");
	}
}

template<IUnknownDerived T>
ComPtr<T>::ComPtr(REFCLSID clsid_something, REFIID iid_something, DWORD clsContext)
{
	if (HRESULT hr = CoCreateInstance(clsid_something, nullptr, clsContext, iid_something, (void**)&this->ptr);
		FAILED(hr))
	{
		core_internals::ThrowSysErr(hr, "CoCreateInstance failed");
	}
}

template<IUnknownDerived T>
ComPtr<T>& ComPtr<T>::operator=(ComPtr&& other) noexcept
{
	this->release();
	std::swap(this->ptr, other.ptr);
	return *this;
}

template<IUnknownDerived T>
void ComPtr<T>::release() noexcept
{
	if (this->ptr) {
		this->ptr->Release();
		this->ptr = nullptr;
	}
}

template<IUnknownDerived T>
template<IUnknownDerived Q>
ComPtr<Q> ComPtr<T>::queryInterface()
{
	ComPtr<Q> ret;
	if (HRESULT hr = this->ptr->QueryInterface(IID_PPV_ARGS(&ret)); FAILED(hr)) {
		core_internals::ThrowSysErr(hr, "IUnknown::QueryInterface failed");
	}
	return ret;
}

template<IUnknownDerived T>
template<IUnknownDerived Q>
ComPtr<Q> ComPtr<T>::queryInterface(REFIID iid_something)
{
	ComPtr<Q> ret;
	if (HRESULT hr = this->ptr->QueryInterface(iid_something, (void**)&ret); FAILED(hr)) {
		core_internals::ThrowSysErr(hr, "IUnknown::QueryInterface failed");
	}
	return ret;
}

}
