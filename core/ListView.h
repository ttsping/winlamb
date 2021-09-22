
#pragma once
#include <optional>
#include <string_view>
#include "ImageList.h"
#include "NativeControl.h"
#include "Menu.h"
#include <CommCtrl.h>

namespace core {

// Non-owning wrapper to native ListView control.
class ListView final : public NativeControl {
public:
	// Expose column methods of a ListView.
	class Columns final {
		friend ListView;
	private:
		const ListView& lv;
		constexpr Columns(const ListView& lv) : lv{lv} { }
		Columns(const Columns&) = delete;
	public:
		[[nodiscard]] size_t count() const;
		const Columns& add(std::wstring_view text, int size) const;
		void stretch(int index) const;
		[[nodiscard]] size_t width(int index) const;
	};

	// Expose item methods of a ListView.
	class Items final {
		friend ListView;
	private:
		const ListView& lv;
		constexpr Items(const ListView& lv) : lv{lv} { }
		Items(const Items&) = delete;
	public:
		[[nodiscard]] size_t count() const;
		UINT add(int iconIdx, std::initializer_list<std::wstring_view> texts) const;
		void remove(int index) const;
		void selectAll(bool doSelect) const;
		std::optional<int> focused() const;
		void setFocused(int index) const;
		bool isVisible(int index) const;
		RECT rect(int index, int lvirPortion = LVIR_BOUNDS) const;
	};

private:
	std::optional<Menu> contextMenu;

public:
	Columns columns;
	Items items;

	ListView(const ListView& other) noexcept
		: NativeControl{other}, contextMenu{contextMenu}, columns{*this}, items{*this} { }
	ListView& operator=(const ListView& other) noexcept;

	explicit ListView(HWND hCtrl, std::optional<Menu> contextMenu = std::nullopt) noexcept
		: NativeControl{hCtrl}, contextMenu{contextMenu},
			columns{*this}, items{*this} { }
	ListView(HWND hParent, int ctrlId, std::optional<Menu> contextMenu = std::nullopt)
		: NativeControl{hParent, ctrlId}, contextMenu{contextMenu},
			columns{*this}, items{*this} { }

	int ctrlId() const { return GetDlgCtrlID(this->hWnd()); }
	bool onWmNotify(LPARAM lp) const;
	void setExtendedStyle(bool set, DWORD exStyle) const;
	void setImageList(const ImageList& imgLst, DWORD normalOrSmall = LVSIL_NORMAL) const;

private:
	void showContextMenu(bool followCursor, bool hasCtrl, bool hasShift) const;
};

}
