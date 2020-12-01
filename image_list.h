/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <Windows.h>
#include <CommCtrl.h>
#include "icon.h"

namespace wl {

/// Manages a native image list (HIMAGELIST), which provides icons for the
/// following controls:
/// - wl::combo_box_ex
/// - wl::list_view
/// - wl::tool_bar
/// - wl::tree_view
///
/// #include <image_list.h>
/// @see @ref ex10
/// @see https://docs.microsoft.com/en-us/windows/win32/controls/image-lists
/// @see http://www.catch22.net/tuts/win32/system-image-list
/// @see http://www.autohotkey.com/docs/commands/ListView.htm
class image_list final {
private:
	HIMAGELIST _hImL = nullptr;
	SIZE _iconSize{};

public:
	/// Destructor. Calls destroy().
	~image_list() { this->destroy(); }

	/// Constructor.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-imagelist_create
	explicit image_list(
		SIZE iconSize,
		WORD capacity = 1, WORD capacityGrowRate = 1, UINT ilcFlags = ILC_COLOR32)
	{
		if (this->_hImL != nullptr) {
			throw std::logic_error("Cannot create image list twice.");
		}

		this->_iconSize = iconSize;
		this->_hImL = ImageList_Create(iconSize.cx, iconSize.cy, ilcFlags,
			static_cast<int>(capacity), static_cast<int>(capacityGrowRate));

		if (this->_hImL == nullptr) {
			throw std::runtime_error("ImageList_Create() failed in image_list::image_list().");
		}
	}

	/// Move constructor.
	image_list(image_list&& other) noexcept { this->operator=(std::move(other)); }  // movable only

	/// Move assignment operator.
	image_list& operator=(image_list&& other) noexcept
	{
		this->destroy();
		std::swap(this->_hImL, other._hImL);
		std::swap(this->_iconSize, other._iconSize);
		return *this;
	}

	/// Returns the underlying HIMAGELIST handle.
	[[nodiscard]] HIMAGELIST himagelist() const noexcept { return this->_hImL; }

	/// Returns the size, in pixels, of the icons within this image list.
	[[nodiscard]] SIZE icon_size() const noexcept { return this->_iconSize; }

	/// Calls ImageList_Destroy().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-imagelist_destroy
	image_list& destroy() noexcept
	{
		if (this->_hImL != nullptr) {
			ImageList_Destroy(this->_hImL);
			this->_hImL = nullptr;
			this->_iconSize = {};
		}
		return *this;
	}

	/// Makes a clone of the given icon and adds it.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-imagelist_addicon
	const image_list& clone_icon_and_add(HICON hIcon) const noexcept
	{
		// The icon can be destroyed right away.
		ImageList_AddIcon(this->_hImL, hIcon);
		return *this;
	}

	/// Makes a clone of the given icon and adds it.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-imagelist_addicon
	const image_list& clone_icon_and_add(const icon& ico) const noexcept
	{
		return this->clone_icon_and_add(ico.hicon());
	}

	/// Loads an icon from the application resource.
	/// @see icon::load_app_resource().
	const image_list& load_from_resource(int iconId, HINSTANCE hInst = nullptr) const
	{
		// Icons loaded from resource files don't need to be destroyed.
		icon tmpIco;
		tmpIco.load_app_resource(iconId, this->_iconSize);
		return this->clone_icon_and_add(tmpIco);
	}

	/// Loads the icon used by Windows Explorer to represent the given file type.
	/// @param fileExtension File extension, ex.: `L"mp3"`.
	/// @see icon::load_shell_file_type().
	const image_list& load_shell_file_type(std::wstring_view fileExtension) const
	{
		icon tmpIco;
		tmpIco.load_shell_file_type(fileExtension, this->_iconSize);
		return this->clone_icon_and_add(tmpIco);
	}

	/// Loads icons used by Windows Explorer to represent the given file type.
	/// @param fileExtension File extensions, ex.: `{L"mp3", L"docx"}`.
	/// @see icon::load_shell_file_type().
	const image_list& load_shell_file_type(
		std::initializer_list<std::wstring_view> fileExtensions) const
	{
		for (std::wstring_view fileExt : fileExtensions) {
			this->load_shell_file_type(fileExt);
		}
		return *this;
	}
};

}//namespace wl