/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <optional>
#include <stdexcept>
#include <vector>
#include <Windows.h>
#include "internal/interfaces.h"
#include "internal/radio_button.h"
#include "internal/msg_wnd_events_ctrl.h"

namespace wl {

/// Manages a group of wl::radio_button controls.
/// A radio button is just a specific type of wl::button, so they share the same events.
///
/// #include <radio_group.h>
///
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/button-types-and-styles#radio-buttons
class radio_group final : public i_resource_control {
public:
	/// Parameters used when creating a radio button with CreateWindowEx().
	/// @see wl::radio_button::create()
	struct create_attrs final {
		std::wstring_view text;
		POINT pos;
		DWORD bStyles = BS_AUTORADIOBUTTON;
		DWORD wStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
		DWORD wExStyles = 0;
	};

	/// Iterator type.
	using iterator = typename std::vector<radio_button>::iterator;

	/// Const iterator type.
	using const_iterator = typename std::vector<radio_button>::const_iterator;

	/// Reverse iterator type.
	using reverse_iterator = typename std::vector<radio_button>::reverse_iterator;

	/// Const reverse iterator type.
	using const_reverse_iterator = typename std::vector<radio_button>::const_reverse_iterator;

private:
	std::vector<radio_button> _items;
	msg::button::wnd_events_rg _events;

public:
	/// Constructor, explicitly defining the control IDs.
	radio_group(i_parent_window* parent, std::initializer_list<WORD> ctrlIds)
	{
		this->_items.reserve(ctrlIds.size()); // if we don't reserve, &r.on() fails because of reallocs
		this->_events._itemEvents.reserve(ctrlIds.size());
		for (WORD ctrlId : ctrlIds) {
			radio_button& r = this->_items.emplace_back(parent, ctrlId);
			this->_events._itemEvents.emplace_back(&r.on()); // fill friend
		}
	}

	/// Constructor, with an auto-generated control IDs.
	/// You must inform the number of radio controls in this group.
	radio_group(i_parent_window* parent, size_t numRadios)
	{
		this->_items.reserve(numRadios); // if we don't reserve, &r.on() fails because of reallocs
		this->_events._itemEvents.reserve(numRadios);
		for (size_t i = 0; i < numRadios; ++i) {
			radio_button& r = this->_items.emplace_back(parent);
			this->_events._itemEvents.emplace_back(&r.on()); // fill friend
		}
	}

	/// Move constructor.
	radio_group(radio_group&&) = default;

	/// Move assignment operator.
	radio_group& operator=(radio_group&&) = default;

	/// Returns the radio_button at the given index.
	/// @warning Throws an exception if index is out of bounds.
	[[nodiscard]] const radio_button& operator[](size_t index) const { return this->_items.at(index); }
	/// Returns the radio_button at the given index.
	/// @warning Throws an exception if index is out of bounds.
	[[nodiscard]] radio_button&       operator[](size_t index)       { return this->_items.at(index); }

	/// Calls CreateWindowEx() for each radio button.
	/// Should be called during parent's WM_CREATE processing.
	///
	/// @param createAttrs Creation attributes for each radio button.
	/// First item will have WS_TABSTOP and WS_GROUP, the others won't.
	///
	/// @warning If the Attributes don't match the same number of radio buttons,
	/// an exception will be thrown.
	///
	/// @see wl::radio_button::create()
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-create
	radio_group& create(std::initializer_list<create_attrs> createAttrs)
	{
		this->_events._itemEvents.clear(); // friend; no longer necessary
		this->_events._itemEvents.shrink_to_fit();

		if (createAttrs.size() != this->_items.size()) {
			throw std::invalid_argument("Number of items in create() doesn't match the number of radio buttons.");
		}

		// https://stackoverflow.com/a/18514815/6923555
		for (auto [iterAtts, iterRads] = std::pair{createAttrs.begin(), this->_items.begin()};
			iterAtts != createAttrs.end();
			++iterAtts, ++iterRads)
		{
			DWORD wStyles = iterAtts->wStyles;
			if (iterAtts == createAttrs.begin()) {
				wStyles |= WS_TABSTOP | WS_GROUP;
			} else {
				wStyles &= ~(WS_TABSTOP | WS_GROUP);
			}

			iterRads->create(iterAtts->text, iterAtts->pos,
				iterAtts->bStyles, wStyles, iterAtts->wExStyles);
		}
		return *this;
	}

private:
	void create_in_dialog() override
	{
		this->_events._itemEvents.clear(); // friend; no longer necessary
		this->_events._itemEvents.shrink_to_fit();

		for (radio_button& r : this->_items) {
			r.create_in_dialog();
		}
	}

public:
	/// Exposes methods to add notification handlers.
	/// @warning If you call this method after the control is created, an exception will be thrown.
	[[nodiscard]] msg::button::wnd_events_rg& on() noexcept { return this->_events; }

	/// Tells if the radio group is empty.
	[[nodiscard]] bool empty() const noexcept { return this->_items.empty(); }

	/// Returns the number of radio controls in this radio group.
	[[nodiscard]] size_t size() const noexcept { return this->_items.size(); }

	/// Returns the radio button with the given control ID, if any.
	[[nodiscard]] std::optional<std::reference_wrapper<const radio_button>>
		by_id(WORD radioId) const noexcept
	{
		return _by_id<const radio_group, const radio_button>(this, radioId);
	}
	/// Returns the radio button with the given control ID, if any.
	[[nodiscard]] std::optional<std::reference_wrapper<radio_button>>
		by_id(WORD radioId) noexcept
	{
		return _by_id<radio_group, radio_button>(this, radioId);
	}

	/// Returns the currently checked radio button, if any.
	[[nodiscard]] std::optional<std::reference_wrapper<const radio_button>>
		checked_radio() const noexcept
	{
		return _checked_radio<const radio_group, const radio_button>(this);
	}
	/// Returns the currently checked radio button, if any.
	[[nodiscard]] std::optional<std::reference_wrapper<radio_button>>
		checked_radio() noexcept
	{
		return _checked_radio<radio_group, radio_button>(this);
	}

	/// Returns the ID of the currently checked radio button, if any.
	[[nodiscard]] std::optional<WORD> checked_id() const noexcept
	{
		auto checkedRadio = this->checked_radio();
		if (checkedRadio.has_value()) {
			return checkedRadio.value().get().ctrl_id();
		}
		return std::nullopt;
	}

	/// Enables or disables all radio buttons at once.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enablewindow
	const radio_group& enable(bool isEnabled) const noexcept
	{
		for (const radio_button& rb : this->_items) {
			rb.enable(isEnabled);
		}
		return *this;
	}

private:
	template<typename thisT, typename retT>
	[[nodiscard]] static std::optional<std::reference_wrapper<retT>>
		_by_id(thisT* thiss, WORD radioId) noexcept
	{
		// https://stackoverflow.com/a/11655924/6923555

		for (retT& rad : thiss->_items) {
			if (rad.ctrl_id() == radioId) {
				return {rad};
			}
		}
		return std::nullopt;
	}

	template<typename thisT, typename retT>
	[[nodiscard]] static std::optional<std::reference_wrapper<retT>>
		_checked_radio(thisT* thiss) noexcept
	{
		for (retT& rad : thiss->_items) {
			if (rad.checked()) {
				return {rad};
			}
		}
		return std::nullopt;
	}

public:
	/// Returns an iterator to the first entry, or end() if map is empty.
	/// @see https://en.cppreference.com/w/cpp/container/vector/begin
	[[nodiscard]] iterator begin() noexcept { return this->_items.begin(); }

	/// Returns an iterator to the element following the last element of the map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/end
	[[nodiscard]] iterator end() noexcept { return this->_items.end(); }

	/// Returns an iterator to the first entry, or end() if map is empty.
	/// @see https://en.cppreference.com/w/cpp/container/vector/begin
	[[nodiscard]] const_iterator begin() const noexcept { return this->_items.begin(); }

	/// Returns an iterator to the element following the last element of the map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/end
	[[nodiscard]] const_iterator end() const noexcept { return this->_items.end(); }

	/// Returns an iterator to the first entry, or end() if map is empty.
	/// @see https://en.cppreference.com/w/cpp/container/vector/begin
	[[nodiscard]] const_iterator cbegin() const noexcept { return this->_items.cbegin(); }

	/// Returns an iterator to the element following the last element of the map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/end
	[[nodiscard]] const_iterator cend() const noexcept { return this->_items.cend(); }

	/// Returns a reverse iterator to the first element of the reversed map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/rbegin
	[[nodiscard]] reverse_iterator rbegin() noexcept { return this->_items.rbegin(); }

	/// Returns a reverse iterator to the element following the last element of the reversed map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/rend
	[[nodiscard]] reverse_iterator rend() noexcept { return this->_items.rend(); }

	/// Returns a reverse iterator to the first element of the reversed map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/rbegin
	[[nodiscard]] const_reverse_iterator rbegin() const noexcept { return this->_items.rbegin(); }

	/// Returns a reverse iterator to the element following the last element of the reversed map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/rend
	[[nodiscard]] const_reverse_iterator rend() const noexcept { return this->_items.rend(); }

	/// Returns a reverse iterator to the first element of the reversed map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/rbegin
	[[nodiscard]] const_reverse_iterator crbegin() const noexcept { return this->_items.crbegin(); }

	/// Returns a reverse iterator to the element following the last element of the reversed map.
	/// @see https://en.cppreference.com/w/cpp/container/vector/rend
	[[nodiscard]] const_reverse_iterator crend() const noexcept { return this->_items.crend(); }
};

}//namespace wl