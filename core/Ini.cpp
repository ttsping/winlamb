
#include "Ini.h"
#include "FileMapped.h"
#include "str.h"
using namespace core;
using std::optional;
using std::reference_wrapper;
using std::vector;
using std::wstring;
using std::wstring_view;

void Ini::load(wstring_view filePath)
{
	this->sections.clear();

	FileMapped fin{filePath, FileMapped::Access::READ};
	wstring contents = str::Parse(fin.hotSpan());
	fin.close();

	Section curSection;

	for (wstring& line : str::SplitLines(contents)) {
		str::Trim(line);
		if (line.empty()) continue;

		if (line[0] == L'[' && line.back() == L']') {
			if (!curSection.name.empty() || !curSection.keys.empty()) {
				this->sections.emplace_back(std::move(curSection));
			}
			curSection.name = line.substr(1, line.length() - 2);

		} else {
			vector<wstring> keyVal = str::Split(line, L"=", {2});
			if (keyVal.size() == 1) continue;			
			curSection.keys.emplace_back(str::Trim(keyVal[0]), str::Trim(keyVal[1]));
		}
	}

	if (!curSection.name.empty() || !curSection.keys.empty()) {
		this->sections.emplace_back(curSection); // last section
	}
}

void Ini::save(wstring_view filePath)
{
	size_t allocSize = 0;

	for (const Section& section : this->sections) {
		allocSize += section.name.length() + 2;
		for (const Key& key : section.keys) {
			allocSize += key.name.length() + 1 + key.val.length() + 4;
		}
		allocSize += 2;
	}

	wstring serialized;
	serialized.reserve(allocSize);

	for (size_t i = 0; i < this->sections.size(); ++i) {
		const Section& section = this->sections[i];

		serialized += L'[';
		serialized += section.name;
		serialized += L"]\r\n";

		for (const Key& key : section.keys) {
			serialized += key.name;
			serialized += L'=';
			serialized += key.val;
			serialized += L"\r\n";
		}

		if (i < this->sections.size() - 1) serialized += L"\r\n";
	}

	File fout{filePath, File::Access::RW_OPEN_OR_CREATE};
	fout.eraseAndWrite(str::SerializeToUtf8(serialized));
}

optional<reference_wrapper<const wstring>> Ini::get(wstring_view section, wstring_view key) const
{
	for (const Section& s : this->sections) {
		if (s.name == section) {
			for (const Key& k : s.keys) {
				if (k.name == key) {
					return {k.val};
				}
			}
		}
	}
	return std::nullopt;
}

optional<reference_wrapper<wstring>> Ini::get(wstring_view section, wstring_view key)
{
	for (Section& s : this->sections) {
		if (s.name == section) {
			for (Key& k : s.keys) {
				if (k.name == key) {
					return {k.val};
				}
			}
		}
	}
	return std::nullopt;
}
