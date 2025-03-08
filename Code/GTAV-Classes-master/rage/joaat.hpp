#pragma once
#include <cstdint>
#include <string_view>
#include <string>
namespace rage {
	using joaat_t = std::uint32_t;
	constexpr char tolower(char c) {
		return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
	}
	constexpr char toUpper(char c) {
		return (c >= 'a' && c <= 'z') ? c + ('A' - 'a') : c;
	}
	inline std::uint32_t joaat(std::string_view str) {
		std::uint32_t hash = 0;
		for (auto c : str)
			hash += tolower(c), hash += (hash << 10), hash ^= (hash >> 6);
		hash += (hash << 3), hash ^= (hash >> 11), hash += (hash << 15);
		return hash;
	}
	inline std::string jooatToChar(uint32_t value) {
		std::string str;
		while (value != 0) {
			char digit = static_cast<char>(value & 0xFF);
			str.insert(str.begin(), digit);
			value >>= 8;
		}
		return str;
	}
}
template <typename string_view_t = std::string_view>
consteval std::uint32_t constexprJoaat(string_view_t str, bool const forceLowerCase = true) {
	std::uint32_t hash{};
	while (*str != '\0') {
		hash += forceLowerCase ? rage::tolower(*str) : *str, ++str;
		hash += hash << 10, hash ^= hash >> 6;
	}
	hash += (hash << 3), hash ^= (hash >> 11), hash += (hash << 15);

	return hash;
}
template <std::size_t arraySize>
consteval std::uint32_t joaatFromCharArray(char const(&str)[arraySize]) {
	std::uint32_t hash{};
	for (int i{}; i != arraySize; ++i) {
		hash += rage::tolower(str[i]);
		hash += hash << 10, hash ^= hash >> 6;
	}
	hash += (hash << 3), hash ^= (hash >> 11), hash += (hash << 15);

	return hash;
}
consteval std::uint32_t operator ""_joaat(const char* str, size_t) { return constexprJoaat(str); }