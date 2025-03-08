#pragma once
#include <cstdint>
#include <type_traits>

enum class eColorValue : uint8_t {
	r,
	b,
	g,
	a
};
class RGBA {
public:
	RGBA() : r(0), g(0), b(0), a(0) {}
	RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : r(red), g(green), b(blue), a(alpha) {}
	RGBA(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue), a(255) {}
public:
	void swapValues(enum class eColorValue swap, enum class eColorValue valueToBeSwapped) {
		std::swap(getColorValueAsUInt(swap), getColorValueAsUInt(valueToBeSwapped));
	}
private:
	uint8_t& getColorValueAsUInt(enum class eColorValue value) {
		switch (value) {
		case eColorValue::r: return r; break;
		case eColorValue::g: return g; break;
		case eColorValue::b: return b; break;
		case eColorValue::a: return a; break;
		}
		return a;
	}
public:
	uint8_t r, g, b, a;
};
struct Color {
	int r, g, b, a;
	Color() : r(0), g(0), b(0), a(0) {}
	Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}
};