/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <cstring>
#include <stdexcept>
#include <vector>
#include <Windows.h>

namespace wl::bin {

/// Analog to string_view, provides a view to a byte array that is defined
/// somewhere else. It's a simple object that can be cheaply copied.
///
/// Ideal for parsing binary data, extracting numbers and strings.
///
/// #include <bin.h>
class byte_view {
private:
	const BYTE* _ptr = nullptr;
	size_t _sz = 0;

public:
	/// Constructor.
	/// @param ptr Pointer to BYTE array that will be the base of the view.
	/// @param numBytes Number of bytes that the view will have.
	byte_view(const BYTE* ptr, size_t numBytes) noexcept :
		_ptr{ptr}, _sz{numBytes} { }

	/// Constructor.
	/// @param src Vector upon the view will be mapped.
	byte_view(const std::vector<BYTE>& src) noexcept :
		_ptr{&src[0]}, _sz{src.size()} { }

	/// Copy constructor.
	byte_view(const byte_view&) = default;

	/// Copy assignment operator.
	byte_view& operator=(const byte_view&) = default;

	/// Returns the number of bytes in the view.
	[[nodiscard]] size_t size() const noexcept { return _sz; }

	/// Returns a reference to the byte at the given index.
	[[nodiscard]] const BYTE& operator[](size_t index) const noexcept { return this->_ptr[index]; }

	/// Creates a new vector with the viewed bytes copied into it.
	[[nodiscard]] std::vector<BYTE> as_new_vector() const
	{
		return std::vector<BYTE>{this->_ptr, this->_ptr + this->_sz};
	}

	/// Tells if the contents of the view are the same of the other data.
	[[nodiscard]] bool eq(std::initializer_list<BYTE> otherData) const noexcept
	{
		if (this->_sz != otherData.size()) return false;

		for (size_t i = 0; i < otherData.size(); ++i) {
			if (this->_ptr[i] != *(otherData.begin() + i)) {
				return false;
			}
		}
		return true;
	}

	/// Tells if the contents of the view are the same of the other data.
	[[nodiscard]] bool eq(const BYTE* otherData) const noexcept
	{
		return std::memcmp(this->_ptr, otherData,
			sizeof(BYTE) * this->_sz) == 0;
	}

	/// Tells if the contents of the view are the same of the other data.
	[[nodiscard]] bool eq(const char* otherData) const noexcept
	{
		return this->eq(reinterpret_cast<const BYTE*>(otherData));
	}

	/// Tells if all the bytes in the view are zero.
	[[nodiscard]] bool is_zeroed() const noexcept
	{
		for (size_t i = 0; i < this->_sz; ++i) {
			if (this->_ptr[i] != 0x00) {
				return false;
			}
		}
		return true;
	}

	/// Creates a new view by slicing the current view.
	/// @param startIndex First byte where the new view will start at.
	/// @param numBytes How many bytes the new view will have.
	[[nodiscard]] byte_view slice(size_t startIndex, size_t numBytes) const
	{
		if (numBytes > startIndex + _sz) {
			throw std::out_of_range("Length is beyond possible in " __FUNCTION__ "().");
		} else if (startIndex >= _sz) { // offset beyond possible
			return byte_view{nullptr, 0};
		}
		return byte_view{this->_ptr + startIndex, numBytes};
	}

	/// Creates a new view by slicing the current view, starting at the given offset.
	/// @param startIndex First byte where the new view will start at.
	[[nodiscard]] byte_view slice_offset(size_t startIndex) const noexcept { return slice(startIndex, this->_sz - startIndex); }

	/// Creates a new view by slicing the current view, truncating it.
	/// @param numBytes How many bytes the new view will have.
	[[nodiscard]] byte_view slice_trunc(size_t numBytes) const noexcept { return slice(0, numBytes); }
};

}//namespace wl::bin