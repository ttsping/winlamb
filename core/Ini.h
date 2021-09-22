
#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace core {

// Parses, manages and serializes an INI file.
class Ini final {
public:
	struct Key final {
		std::wstring name;
		std::wstring val;
	};

	struct Section final {
		std::wstring name;
		std::vector<Key> keys;
	};

	std::vector<Section> sections;

	Ini(Ini&& other) = default;
	Ini& operator=(Ini&& other) = default;

	Ini() = default;
	explicit Ini(std::wstring_view filePath);

	void save(std::wstring_view filePath);
	[[nodiscard]] std::optional<std::reference_wrapper<const std::wstring>> get(std::wstring_view section, std::wstring_view key) const;
	[[nodiscard]] std::optional<std::reference_wrapper<std::wstring>> get(std::wstring_view section, std::wstring_view key);
};

}
