/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once

#include <algorithm>
#include <vector>
#include "internal/lambda_type.h"

/// Vector utilities.
///
/// #include <vec.h>
namespace wl::vec {

/// Appends a vector to another.
/// @see https://en.cppreference.com/w/cpp/container/vector/insert
template<typename T>
void append(std::vector<T>& dest, const std::vector<T>& other)
{
	dest.insert(dest.end(), other.begin(), other.end());
}

/// Appends items to a vector.
/// @see https://en.cppreference.com/w/cpp/container/vector/insert
template<typename T>
void append(std::vector<T>& dest, std::initializer_list<T> values)
{
	dest.insert(dest.end(), values.begin(), values.end());
}

/// Deletes from vector all elements where lambda returns true, by implementing
/// the erase-remove idiom.
/// @tparam T Type of vector elements.
/// @tparam F `std::function<bool(const T&)>`
/// @param v Vector to be processed.
/// @param func: `[](const T& elem) -> bool {}`
/// @see https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom
/// @see https://en.cppreference.com/w/cpp/container/vector/erase
/// @see https://en.cppreference.com/w/cpp/algorithm/remove
template<typename T, typename F>
auto delete_if(std::vector<T>& v, F&& func)
	-> WINLAMB_LAMBDA_TYPE(func, bool(const T&), void)
{
	v.erase(
		std::remove_if(v.begin(), v.end(), std::forward<F>(func)),
		v.end());
}

}//namespace wl::vec