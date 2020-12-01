/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <system_error>
#include <vector>
#include <Windows.h>

namespace wl {

/// Helps building an accelerator table, which defines keyboard shortcuts.
///
/// #include <accel_table.h>
/// @see @ref ex11
/// @see https://docs.microsoft.com/en-us/windows/win32/learnwin32/accelerator-tables
class accel_table final {
	std::vector<ACCEL> _accels;
	HACCEL _hAccel = nullptr;

public:
	/// Modifiers for accelerators with combined keys.
	enum class modif : BYTE {
		NONE  = FVIRTKEY,
		CTRL  = FVIRTKEY | FCONTROL,
		SHIFT = FVIRTKEY | FSHIFT,
		ALT   = FVIRTKEY | FALT,
	};

	/// Destructor. Calls destroy().
	~accel_table() { this->destroy(); }

	/// Default constructor.
	accel_table() = default;

	/// Move constructor
	accel_table(accel_table&& other) noexcept { operator=(std::move(other)); }

	/// Equality operator.
	[[nodiscard]] bool operator==(const accel_table& other) const noexcept { return this->_hAccel == other._hAccel; }
	/// Inequality operator.
	[[nodiscard]] bool operator!=(const accel_table& other) const noexcept { return !this->operator==(other); }

	/// Move assignment operator.
	accel_table& operator=(accel_table&& other) noexcept
	{
		this->destroy();
		std::swap(this->_accels, other._accels);
		std::swap(this->_hAccel, other._hAccel);
		return *this;
	}

	/// Adds a new character accelerator, with a specific command ID.
	accel_table& add_char(wchar_t ch, modif modifiers, WORD cmdId)
	{
		if (this->_hAccel != nullptr) {
			throw std::logic_error("Cannot add character after accelerator table was built, with " __FUNCTION__ "().");
		}

		CharUpperBuffW(&ch, 1);
		this->_accels.emplace_back(
			ACCEL{static_cast<BYTE>(modifiers), ch, cmdId});
		return *this;
	}

	/// Adds a new virtual key accelerator, with a specific command ID.
	accel_table& add_key(WORD vKey, modif modifiers, WORD cmdId)
	{
		if (this->_hAccel != nullptr) {
			throw std::logic_error("Cannot add virtual key after accelerator table was built, with " __FUNCTION__ "().");
		}

		this->_accels.emplace_back(
			ACCEL{static_cast<BYTE>(modifiers), vKey, cmdId});
		return *this;
	}

	/// Calls DestroyAcceleratorTable().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroyacceleratortable
	void destroy() noexcept
	{
		if (this->_hAccel != nullptr) {
			DestroyAcceleratorTable(this->_hAccel);
			this->_hAccel = nullptr;
			this->_accels.clear();
		}
	}

	/// Builds the accelerator table, if not yet, and returns HACCEL handle.
	/// After you call this method, no further accelerators can be added.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createacceleratortablew
	[[nodiscard]] HACCEL haccel()
	{
		if (this->_hAccel == nullptr && !this->_accels.empty()) { // build only once
			this->_hAccel = CreateAcceleratorTableW(
				&this->_accels[0], static_cast<int>(this->_accels.size()) );

			if (this->_hAccel == nullptr) {
				throw std::system_error(GetLastError(), std::system_category(),
					"CreateAcceleratorTable() failed in " __FUNCTION__ "().");
			}

			this->_accels.clear();
			this->_accels.shrink_to_fit(); // not needed anymore
		}

		return this->_hAccel;
	}
};

}//namespace wl

DEFINE_ENUM_FLAG_OPERATORS(wl::accel_table::modif)