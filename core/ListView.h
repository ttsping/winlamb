
#pragma once
#include "NativeControl.h"
#include <string_view>

namespace core {

// ListView native control.
class ListView final : public NativeControl {
public:
	// Expose column methods of a ListView.
	class Columns final {
		friend ListView;
	private:
		ListView& lv;
		Columns(ListView& lv) : lv{lv} { }
	public:
		UINT count() const;
		const Columns& add(std::wstring_view text, int size) const;
		void stretch(int index) const;
		UINT width(int index) const;
	};

	// Expose item methods of a ListView.
	class Items final {
		friend ListView;
	private:
		ListView& lv;
		Items(ListView& lv) : lv{lv} { }
	public:
		UINT count() const;
		UINT add(int iconIdx, std::initializer_list<std::wstring_view> texts) const;
		void remove(int index) const;
	};

	Columns columns{*this};
	Items items{*this};

	explicit ListView(HWND hCtrl) : NativeControl{hCtrl} { }
	ListView(HWND hDlg, int ctrlId) : NativeControl{GetDlgItem(hDlg, ctrlId)} { }
};

}
