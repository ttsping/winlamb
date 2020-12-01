/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <functional>
#include <type_traits>

// Validates the lambda type, and defines the function return type.
// https://stackoverflow.com/a/64030013/6923555
#define WINLAMB_LAMBDA_TYPE(fparam, ftype, rettype) \
	std::enable_if_t< \
		std::is_same_v< \
			decltype(std::function{std::forward<F>(fparam)}), \
			std::function<ftype> \
		>, rettype \
	>