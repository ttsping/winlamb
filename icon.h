/*
 * Part of WinLamb - Win32 API Lambda Library
 * https://github.com/rodrigocfd/winlamb
 * This library is released under the MIT License.
 */

#pragma once
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <Windows.h>
#include <CommCtrl.h>
#include <commoncontrols.h> // IID_IImageList
#include "internal/interfaces.h"
#include "com.h"
#include "str.h"

namespace wl {

/// Manages an HICON resource.
///
/// #include <icon.h>
/// @see https://docs.microsoft.com/en-us/windows/win32/uxguide/vis-icons
class icon final {
private:
	HICON _hIcon = nullptr;

public:
	/// Destructor. Calls destroy().
	~icon() { this->destroy(); }

	/// Default constructor.
	icon() = default;

	/// Move constructor.
	icon(icon&& other) noexcept { this->operator=(std::move(other)); }

	/// Move assignment operator.
	icon& operator=(icon&& other) noexcept
	{
		this->destroy();
		std::swap(this->_hIcon, other._hIcon);
		return *this;
	}

	/// Returns the underlying HICON handle.
	[[nodiscard]] HICON hicon() const noexcept { return this->_hIcon; }

	/// Calls DestroyIcon().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroyicon
	icon& destroy() noexcept
	{
		if (this->_hIcon) {
			DestroyIcon(this->_hIcon);
			this->_hIcon = nullptr;
		}
		return *this;
	}

	/// Draws an icon into a wl::label control; the icon can be safely destroyed then.
	/// @note On the resource editor, for the control, change "Type" property to "Icon".
	/// @see https://docs.microsoft.com/en-us/windows/win32/controls/stm-setimage
	const icon& draw_in_label(const i_window& labelCtrl) const noexcept
	{
		SendMessageW(labelCtrl.hwnd(), STM_SETIMAGE, IMAGE_ICON,
			reinterpret_cast<LPARAM>(this->_hIcon));
		return *this;
	}

	/// Loads an icon from the application resource.
	/// @param iconId ID of the icon resource to be loaded, usually declared in a .rc file.
	/// @param iconSize Size of the icon to be loaded, in pixels.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadimagew
	icon& load_app_resource(int iconId, SIZE iconSize)
	{
		return this->_raw_load_resource(iconId, iconSize, GetModuleHandleW(nullptr));
	}

	/// Loads the icon used by Windows Explorer to represent the given file type.
	/// @param fileExtension File extension, like L"mp3".
	/// @param iconSize Size of the icon to be loaded, in pixels.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shgetfileinfow
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shgetimagelist
	icon& load_shell_file_type(std::wstring_view fileExtension, SIZE iconSize)
	{
		this->destroy();

		wchar_t extens[16]{}; // arbitrary length
		lstrcpyW(extens, (fileExtension[0] == L'.') ? L"*" : L"*."); // prepend dot if it doesn't have
		lstrcatW(extens, fileExtension.data());

		com::lib   comLib{com::lib::init::NOW};
		SHFILEINFO shfi{};
		int        shilFlag = _size_to_shil(iconSize);

		if (shilFlag == SHIL_SMALL || shilFlag == SHIL_LARGE) { // http://stackoverflow.com/a/28015423
			if (DWORD_PTR r = SHGetFileInfoW(extens, FILE_ATTRIBUTE_NORMAL,
					&shfi, sizeof(shfi), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON |
					(shilFlag == SHIL_SMALL ? SHGFI_SMALLICON : SHGFI_LARGEICON) );
				r == 0)
			{
				throw std::runtime_error(
					"SHGetFileInfo() failed in " __FUNCTION__ "().");
			}
			this->_hIcon = shfi.hIcon;

		} else {
			IImageList* pImgList = nullptr; // http://stackoverflow.com/a/30496252

			if (HRESULT hr = SHGetImageList(shilFlag, IID_IImageList,
					reinterpret_cast<void**>(&pImgList));
				FAILED(hr))
			{
				throw std::system_error(hr, std::system_category(),
					"SHGetImageList() failed in " __FUNCTION__ "().");
			}

			if (DWORD_PTR r = SHGetFileInfoW(extens, FILE_ATTRIBUTE_NORMAL,
					&shfi, sizeof(shfi), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX);
				r == 0)
			{
				throw std::runtime_error(
					"SHGetFileInfo() failed in " __FUNCTION__ "().");
			}
			this->_hIcon = ImageList_GetIcon(
				reinterpret_cast<HIMAGELIST>(pImgList), shfi.iIcon, ILD_NORMAL);
		}

		return *this;
	}

	/// Loads a system icon resource.
	/// @param iconId ID of the system icon resource to be loaded.
	/// @param iconSize Size of the icon to be loaded, in pixels.
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadimagew
	icon& load_sys_resource(int iconId, SIZE iconSize)
	{
		return this->_raw_load_resource(iconId, iconSize, nullptr);
	}

	/// Retrieves the size dynamically with GetObject().
	/// @see https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-getobjectw
	[[nodiscard]] SIZE calc_size() const
	{
		SIZE sz{}; // http://stackoverflow.com/a/13923853

		if (this->_hIcon != nullptr) {
			BITMAP   bmp{};
			ICONINFO nfo{};

			if (GetIconInfo(this->_hIcon, &nfo) == FALSE) {
				throw std::system_error(GetLastError(), std::system_category(),
					"GetIconInfo() failed in icon::calc_size().");
			}

			if (nfo.hbmColor != nullptr) {
				int nWrittenBytes = GetObjectW(nfo.hbmColor, sizeof(bmp), &bmp);
				if (nWrittenBytes > 0) {
					sz.cx = bmp.bmWidth;
					sz.cy = bmp.bmHeight;
					//myinfo.nBitsPerPixel = bmp.bmBitsPixel;
				}
			} else if (nfo.hbmMask != nullptr) {
				int nWrittenBytes = GetObjectW(nfo.hbmMask, sizeof(bmp), &bmp);
				if (nWrittenBytes > 0) {
					sz.cx = bmp.bmWidth;
					sz.cy = bmp.bmHeight / 2;
					//myinfo.nBitsPerPixel = 1;
				}
			}

			if (nfo.hbmColor != nullptr) DeleteObject(nfo.hbmColor);
			if (nfo.hbmMask != nullptr) DeleteObject(nfo.hbmMask);
		}

		return sz;
	}

private:
	icon& _raw_load_resource(int iconId, SIZE iconSize, HINSTANCE hInst)
	{
		this->destroy();

		this->_hIcon = static_cast<HICON>(
			LoadImageW(hInst, MAKEINTRESOURCEW(iconId), IMAGE_ICON,
				iconSize.cx, iconSize.cy, LR_DEFAULTCOLOR));

		if (!this->_hIcon) {
			throw std::system_error(GetLastError(), std::system_category(),
				"LoadImage() failed in icon::_raw_load_resource().");
		}
	}

	// Converts the size into SHIL flag, used by SHGetImageList().
	[[nodiscard]] static int _size_to_shil(SIZE iconSize)
	{
		if (iconSize.cx == 16 && iconSize.cy == 16) {
			return SHIL_SMALL;
		} else if (iconSize.cx == 32 && iconSize.cy == 32) {
			return SHIL_LARGE;
		} else if (iconSize.cx == 48 && iconSize.cy == 48) {
			return SHIL_EXTRALARGE;
		} else if (iconSize.cx == 256 && iconSize.cy == 256) {
			return SHIL_JUMBO;
		}

		throw std::invalid_argument(
			str::unicode_to_ansi(
				str::format(
					L"Invalid icon size for SHIL flag: %dx%d.",
						iconSize.cx, iconSize.cy) ));
	}
};

}//namespace wl