
#pragma once
#include <string_view>
#include "NativeControl.h"
#include "ImageList.h"
#include <CommCtrl.h>

namespace core {

// ListView native control.
class ListView final : public NativeControl {
public:
	// Expose column methods of a ListView.
	class Columns final {
		friend ListView;
	private:
		ListView& lv;
		constexpr Columns(ListView& lv) : lv{lv} { }
	public:
		[[nodiscard]] UINT count() const;
		const Columns& add(std::wstring_view text, int size) const;
		void stretch(int index) const;
		[[nodiscard]] UINT width(int index) const;
	};

	// Expose item methods of a ListView.
	class Items final {
		friend ListView;
	private:
		ListView& lv;
		constexpr Items(ListView& lv) : lv{lv} { }
	public:
		[[nodiscard]] UINT count() const;
		UINT add(int iconIdx, std::initializer_list<std::wstring_view> texts) const;
		void remove(int index) const;
	};

	Columns columns{*this};
	Items items{*this};

	explicit constexpr ListView(HWND hCtrl) : NativeControl{hCtrl} { }
	ListView(HWND hDlg, int ctrlId) : NativeControl{GetDlgItem(hDlg, ctrlId)} { }

	void setExtendedStyle(bool set, DWORD exStyle) const;
	void setImageList(const ImageList& imgLst, DWORD normalOrSmall = LVSIL_NORMAL) const;
};

}
