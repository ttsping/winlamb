/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <vector>
#include <Windows.h>
#include "internal/base_parent_subclass.h"
#include "internal/interfaces.h"
#include "internal/msg_wm.h"
#include "radio_group.h"

namespace wl {

/// When the parent window is resized, automatically adjusts position and size of
/// child controls.
///
/// #include <resizer.h>
/// @see @ref ex04
/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-begindeferwindowpos
class resizer final {
public:
	/// Determines if the child controls will be adjusted automatically when the
	/// parent is resized.
	enum class adjust {
		/// The child controls will be automatically resized when the parend is
		/// resized. This is done by subclassing the parent window.
		AUTO,
		/// The child controls will be resized only when you manually call
		/// adjust_to_fit_parent(), preferably during parent's WM_SIZE processing.
		MANUAL
	};

	/// Horizontal behavior of a control when the parent window is resized.
	enum class horz {
		/// Control width is fixed; x-position moves around, anchored.
		REPOS,
		/// Control width stretches; control doesn't move in x-position.
		RESIZE,
		/// Control width is fixed; control doesn't move in x-position.
		NOTHING
	};

	/// Vertical behavior of a control when the parent window is resized.
	enum class vert {
		/// Control height is fixed; y-position moves around, anchored.
		REPOS,
		/// Control height stretches; control doesn't move in y-position.
		RESIZE,
		/// Control height is fixed; control doesn't move in y-position.
		NOTHING
	};

private:
	struct ctrl final {
		HWND hChild;
		RECT rcOrig;   // original coordinates relative to parent
		horz horizontalBehavior; // horizontal behavior
		vert verticalBehavior; // vertical hehavior
	};

	adjust _adjustAction;
	_wli::base_parent_subclass<WM_SIZE, msg::wm_size> _parentResize;
	std::vector<ctrl> _ctrls;
	SIZE _szOrig{};

public:
	/// Constructor.
	explicit resizer(adjust adjustAction = adjust::AUTO)
		: _adjustAction{adjustAction} { }

	/// Move constructor.
	resizer(resizer&& other) = default;

	/// Move assignment operator.
	resizer& operator=(resizer&& other) = default;

	/// Adds a child control to be resized when parent resizes.
	/// @param horizontalBehavior Horizontal behavior when the parent is resized.
	/// @param verticalBehavior Vertical behavior when the parent is resized.
	/// @param child Child control.
	resizer& add(
		horz horizontalBehavior, vert verticalBehavior,
		const i_resizable_control& child)
	{
		this->_add_one(horizontalBehavior, verticalBehavior, child.hwnd());
		return *this;
	}

	/// Adds multiple child controls to be resized when parent resizes.
	/// @param horizontalBehavior Horizontal behavior when the parent is resized.
	/// @param verticalBehavior Vertical behavior when the parent is resized.
	/// @param children Children controls.
	resizer& add(
		horz horizontalBehavior, vert verticalBehavior,
		std::initializer_list<std::reference_wrapper<const i_resizable_control>> children)
	{
		this->_ctrls.reserve(this->_ctrls.size() + children.size());
		for (const i_resizable_control& child : children) {
			this->add(horizontalBehavior, verticalBehavior, child);
		}
		return *this;
	}

	/// Adds all wl::radio_button controls of a wl::radio_group to be resized when parent resizes.
	/// @param horizontalBehavior Horizontal behavior when the parent is resized.
	/// @param verticalBehavior Vertical behavior when the parent is resized.
	/// @param radioGroup Radio group.
	resizer& add(
		horz horizontalBehavior, vert verticalBehavior,
		const radio_group& radioGroup)
	{
		this->_ctrls.reserve(this->_ctrls.size() + radioGroup.size());
		for (const radio_button& rb : radioGroup) {
			this->add(horizontalBehavior, verticalBehavior, rb);
		}
		return *this;
	}

	/// Adds all wl::radio_button controls of multiple wl::radio_group to be resized when parent resizes.
	/// @param horizontalBehavior Horizontal behavior when the parent is resized.
	/// @param verticalBehavior Vertical behavior when the parent is resized.
	/// @param radioGroups Radio groups.
	resizer& add(
		horz horizontalBehavior, vert verticalBehavior,
		std::initializer_list<std::reference_wrapper<const radio_group>> radioGroups)
	{
		size_t numCtrls = 0;
		for (const radio_group& rg : radioGroups) {
			numCtrls += rg.size();
		}
		this->_ctrls.reserve(this->_ctrls.size() + numCtrls);

		for (const radio_group& rg : radioGroups) {
			this->add(horizontalBehavior, verticalBehavior, rg);
		}
		return *this;
	}

	/// Adds a child control to be resized when the parent resizes.
	/// @param horizontalBehavior Horizontal behavior when the parent is resized.
	/// @param verticalBehavior Vertical behavior when the parent is resized.
	/// @param parent Parent window.
	/// @param ctrlId Child control ID.
	resizer& add(
		horz horizontalBehavior, vert verticalBehavior,
		const i_window* parent, int ctrlId)
	{
		if (parent == nullptr || parent->hwnd() == nullptr) {
			throw std::invalid_argument("Cannot resize a control whose parent is null.");
		}

		this->_add_one(horizontalBehavior, verticalBehavior, GetDlgItem(parent->hwnd(), ctrlId));
		return *this;
	}

	/// Adds multiple children controls to be resized when the parent resizes.
	/// @param horizontalBehavior Horizontal behavior when the parent is resized.
	/// @param verticalBehavior Vertical behavior when the parent is resized.
	/// @param parent Parent window.
	/// @param ctrlId Children controls IDs.
	resizer& add(
		horz horizontalBehavior, vert verticalBehavior,
		const i_window* parent, std::initializer_list<int> ctrlIds)
	{
		for (int ctrlId : ctrlIds) {
			this->add(horizontalBehavior, verticalBehavior, parent, ctrlId);
		}
		return *this;
	}

	/// Updates controls. Intended to be called with parent's WM_SIZE processing.
	///
	/// If you are using wl::resizer::adjust::AUTO, there's no need to call this
	/// method.
	/// @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-size
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-begindeferwindowpos
	void adjust_to_fit_parent(msg::wm_size p) noexcept
	{
		if (this->_ctrls.empty() || p.is_minimized()) {
			return; // if no controls, or if minimized, no need to process
		}

		HDWP hdwp = BeginDeferWindowPos(static_cast<int>(this->_ctrls.size()));
		for (const ctrl& co : this->_ctrls) {
			UINT uFlags = SWP_NOZORDER;
			if (co.horizontalBehavior == horz::REPOS && co.verticalBehavior == vert::REPOS) { // reposition both vert & horz
				uFlags |= SWP_NOSIZE;
			} else if (co.horizontalBehavior == horz::RESIZE && co.verticalBehavior == vert::RESIZE) { // resize both vert & horz
				uFlags |= SWP_NOMOVE;
			}

			DeferWindowPos(hdwp, co.hChild, nullptr,
				co.horizontalBehavior == horz::REPOS
					? p.client_area_size().cx - this->_szOrig.cx + co.rcOrig.left
					: co.rcOrig.left, // keep original pos
				co.verticalBehavior == vert::REPOS
					? p.client_area_size().cy - this->_szOrig.cy + co.rcOrig.top
					: co.rcOrig.top, // keep original pos
				co.horizontalBehavior == horz::RESIZE
					? p.client_area_size().cx - this->_szOrig.cx + co.rcOrig.right - co.rcOrig.left
					: co.rcOrig.right - co.rcOrig.left, // keep original width
				co.verticalBehavior == vert::RESIZE
					? p.client_area_size().cy - this->_szOrig.cy + co.rcOrig.bottom - co.rcOrig.top
					: co.rcOrig.bottom - co.rcOrig.top, // keep original height
				uFlags);
		}
		EndDeferWindowPos(hdwp);
	}

private:
	void _add_one(horz horizontalBehavior, vert verticalBehavior, HWND hChild)
	{
		if (hChild == nullptr) {
			throw std::invalid_argument("Cannot resize a control whose HWND is null.");
		}

		HWND hParent = GetParent(hChild);
		if (this->_ctrls.empty()) { // first call to add()
			if (this->_adjustAction == adjust::AUTO) {
				this->_parentResize.subclass(hParent, [this](msg::wm_size p) {
					this->adjust_to_fit_parent(p);
				});
			}

			RECT rcP{};
			GetClientRect(hParent, &rcP);
			this->_szOrig.cx = rcP.right;
			this->_szOrig.cy = rcP.bottom; // save original size of parent
		}

		RECT rcCtrl{};
		GetWindowRect(hChild, &rcCtrl);
		this->_ctrls.push_back({hChild, rcCtrl, horizontalBehavior, verticalBehavior});
		ScreenToClient(hParent, reinterpret_cast<POINT*>(&this->_ctrls.back().rcOrig)); // client coordinates relative to parent
		ScreenToClient(hParent, reinterpret_cast<POINT*>(&this->_ctrls.back().rcOrig.right));
	}
};


}//namespace wl