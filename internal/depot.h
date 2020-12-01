/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <functional>
#include <vector>
#include <Windows.h>
#include "lambda_type.h"
#include "msg_wm.h"

namespace _wli {

// Keeps message identifiers and their respective lambda handlers.
template<typename idT, typename codeT = unsigned int, codeT defcodeV = 0>
class depot final {
private:
	struct handler final {
		idT   id;   // message ID
		codeT code; // notification code, needed by WM_COMMAND and WM_NOTIFY
		std::function<LRESULT(wl::msg::wm)> func; // user handler; returned INT_PTR is convertible to LRESULT

		template<typename F>
		handler(idT newId, codeT newCode, F&& newFunc) noexcept // needed for emplace_back(id, func)
			: id{newId}, code{newCode}, func{std::forward<F>(newFunc)} { }
	};

	std::vector<handler> _handlers;

public:
	depot()
	{
		this->_handlers.emplace_back(idT{}, codeT{}, nullptr); // add 1st element, which is just room for sentinel
	}

	[[nodiscard]] bool empty() const noexcept
	{
		return this->_handlers.size() <= 1; // sentinel element always present
	}

	// One ID, one code.
	template<typename F>
	auto add(idT id, codeT code, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(wl::msg::wm), void)
	{
		// https://stackoverflow.com/a/64868051/6923555
		this->_handlers.emplace_back(id, code, std::forward<F>(func));
	}

	// One ID, default code.
	template<typename F>
	auto add(idT id, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(wl::msg::wm), void)
	{
		this->add(id, defcodeV, std::forward<F>(func));
	}

	// Multiple IDs, one code.
	template<typename F>
	auto add(std::initializer_list<idT> ids, codeT code, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(wl::msg::wm), void)
	{
		auto iterIds = ids.begin();
		this->add(*iterIds++, code, std::forward<F>(func)); // store user func once, under 1st ID
		size_t funcIdx = this->_handlers.size() - 1; // internal index of stored user func
		for (; iterIds != ids.end(); ++iterIds) { // for all remaining IDs
			if (*iterIds == *ids.begin()) continue; // avoid overwriting
			this->add(*iterIds, code, [this, funcIdx](wl::msg::wm p) -> LRESULT { // store light wrapper to 1st func
				return this->_handlers[funcIdx].func(p);
			});
		}
	}

	// Multiple IDs, default code.
	template<typename F>
	auto add(std::initializer_list<idT> ids, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(wl::msg::wm), void)
	{
		this->add(ids, defcodeV, std::forward<F>(func));
	}

	// One ID, multiple codes.
	template<typename F>
	auto add(idT id, std::initializer_list<codeT> codes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(wl::msg::wm), void)
	{
		auto iterCodes = codes.begin();
		this->add(id, *iterCodes++, std::forward<F>(func)); // store user func once, under 1st ID
		size_t funcIdx = this->_handlers.size() - 1; // internal index of stored user func
		for (; iterCodes != codes.end(); ++iterCodes) { // for all remaining IDs
			if (*iterCodes == *codes.begin()) continue; // avoid overwriting
			this->add(id, *iterCodes, [this, funcIdx](wl::msg::wm p) -> LRESULT { // store light wrapper to 1st func
				return this->_handlers[funcIdx].func(p);
			});
		}
	}

	// Multiple IDs, multiple codes.
	template<typename F>
	auto add(std::initializer_list<idT> ids, std::initializer_list<codeT> codes, F&& func)
		-> WINLAMB_LAMBDA_TYPE(func, LRESULT(wl::msg::wm), void)
	{
		auto iterIds = ids.begin();
		auto iterCodes = codes.begin();
		this->add(*iterIds, *iterCodes++, std::forward<F>(func)); // store user func once, under 1st IDs
		size_t funcIdx = this->_handlers.size() - 1; // internal index of stored user func
		for (; iterIds != ids.end(); ++iterIds) { // for all remaining IDs 1
			for (; iterCodes != codes.end(); ++iterCodes) { // for all remaining IDs 2
				if (*iterIds == *ids.begin() && *iterCodes == *codes.begin()) continue; // avoid overwriting
				this->add(*iterIds, *iterCodes, [this, funcIdx](wl::msg::wm p) -> LRESULT { // store light wrapper to 1st func
					return this->_handlers[funcIdx].func(p);
				});
			}
			iterCodes = codes.begin();
		}
	}

	[[nodiscard]] std::function<LRESULT(wl::msg::wm)>*
		find(idT id, codeT code = defcodeV) noexcept
	{
		this->_handlers[0].id = id; // sentinel for reverse linear search
		this->_handlers[0].code = code;
		handler* revRunner = &this->_handlers.back(); // pointer to last element
		while (revRunner->id != id || revRunner->code != code) --revRunner;
		return revRunner == &this->_handlers[0]
			? nullptr // if we stopped only at 1st element (sentinel), id wasn't found
			: &revRunner->func; // handler found
	}
};

}//namespace _wli