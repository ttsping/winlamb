
#pragma once
#include <optional>
#include <string_view>
#include <vector>
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
		void add(std::initializer_list<std::pair<std::wstring_view, int>> titlesAndSizes) const;
		[[nodiscard]] size_t count() const;
		void setTitle(int index, std::wstring_view text) const;
		void setWidth(int index, size_t width) const;
		void stretch(int index) const;
		[[nodiscard]] std::wstring title(int index) const;
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
		int add(std::initializer_list<std::wstring_view> texts) const { return this->add(-1, texts); }
		int add(int iconIndex, std::initializer_list<std::wstring_view> texts) const;
		[[nodiscard]] size_t count() const;
		void ensureVisible(int index) const;
		[[nodiscard]] std::optional<int> find(std::wstring_view caseInsensText) const;
		[[nodiscard]] std::optional<int> focused() const;
		[[nodiscard]] int iconIndex(int itemIndex) const;
		[[nodiscard]] bool isSelected(int index) const;
		[[nodiscard]] bool isVisible(int index) const;
		[[nodiscard]] LPARAM lParam(int index) const;
		[[nodiscard]] RECT rect(int index, int lvirPortion = LVIR_BOUNDS) const;
		void remove(int index) const;
		void removeAll() const;
		void selectAll(bool doSelect) const;
		[[nodiscard]] std::vector<int> selected() const;
		[[nodiscard]] size_t selectedCount() const;
		void setFocused(int index) const;
		void setIconIndex(int itemIndex, int iconIndex) const;
		void setLParam(int index, LPARAM lp) const;
		void setSelected(const std::vector<int>& indexes) const;
		void setText(int itemIndex, int columnIndex, std::wstring_view text) const;
		[[nodiscard]] std::wstring text(int itemIndex, int columnIndex) const;
	};

private:
	std::optional<Menu> contextMenu;

public:
	Columns columns;
	Items items;

	ListView(const ListView& other)
		: NativeControl{other}, contextMenu{contextMenu}, columns{*this}, items{*this} { }
	ListView& operator=(const ListView& other);

	explicit ListView(HWND hCtrl, std::optional<Menu> contextMenu = std::nullopt)
		: NativeControl{hCtrl}, contextMenu{contextMenu},
			columns{*this}, items{*this} { }
	ListView(HWND hParent, int ctrlId, std::optional<Menu> contextMenu = std::nullopt)
		: NativeControl{hParent, ctrlId}, contextMenu{contextMenu},
			columns{*this}, items{*this} { }

	[[nodiscard]] int ctrlId() const;
	bool onWmNotify(LPARAM lp) const;
	void setExtendedStyle(bool set, DWORD exStyle) const;
	void setImageList(const ImageList& imgLst, DWORD normalOrSmall = LVSIL_NORMAL) const;
	void setRedraw(bool doRedraw) const;
	void setView(DWORD lvView) const;
	[[nodiscard]] DWORD view() const;

private:
	void showContextMenu(bool followCursor, bool hasCtrl, bool hasShift) const;
};

}
