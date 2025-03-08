#pragma once
#include "../Common.hpp"

namespace rage {
	template<typename T>
	union vector2
	{
		T data[2];
		struct { T x, y; };
	};

	template<typename T>
	union vector3
	{
		T data[3];
		struct { T x, y, z; };
	};

	template<typename T>
	union vector4
	{
		T data[4];
		struct { T x, y, z, w; };
	};

	template<typename T>
	union matrix34
	{
		T data[3][4];
		struct { struct { T x, y, z, w; } rows[3]; };
	};

	template<typename T>
	union matrix44
	{
		T data[4][4];
		struct { struct { T x, y, z, w; } rows[4]; };
	};

	using fvector2 = vector2<float>;
	using fvector3 = vector3<float>;
	using fvector4 = vector4<float>;
	using fmatrix34 = matrix34<float>;
	using fmatrix44 = matrix44<float>;

	class scrVector
	{
	public:
		scrVector() = default;

		scrVector(rage::fvector3 vec) :
			x(vec.x), y(vec.y), z(vec.z)
		{
		}

		scrVector(float x, float y, float z) :
			x(x), y(y), z(z)
		{
		}

		scrVector operator+(const scrVector& other)
		{
			scrVector vec;
			vec.x = this->x + other.x;
			vec.y = this->y + other.y;
			vec.z = this->z + other.z;
			return vec;
		}

		scrVector operator-(const scrVector& other)
		{
			scrVector vec;
			vec.x = this->x - other.x;
			vec.y = this->y - other.y;
			vec.z = this->z - other.z;
			return vec;
		}

		scrVector operator*(const scrVector& other)
		{
			scrVector vec;
			vec.x = this->x * other.x;
			vec.y = this->y * other.y;
			vec.z = this->z * other.z;
			return vec;
		}

		scrVector operator*(const float& other)
		{
			scrVector vec;
			vec.x = this->x * other;
			vec.y = this->y * other;
			vec.z = this->z * other;
			return vec;
		}

		bool operator==(const scrVector& other)
		{
			return this->x == other.x && this->y == other.y && this->z == other.z;
		}

		bool operator!=(const scrVector& other)
		{
			return this->x != other.x || this->y != other.y || this->z != other.z;
		}

		alignas(8) float x{};
		alignas(8) float y{};
		alignas(8) float z{};

		std::string to_string() const
		{
			std::stringstream ss;
			ss << *this;
			return ss.str();
		}

		friend std::ostream& operator<<(std::ostream& os, const scrVector& vec)
		{
			os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
			return os;
		}
	};
}

#pragma pack(push, 8)
class Vector2 {
public:
	Vector2() : x(0.f), y(0.f) {}
	Vector2(float x, float y) : x(x), y(y) {}
	alignas(sizeof(double)) float x;
	float y;
	Vector2 operator*(const float amount) {
		return { x * amount, y * amount };
	}
	Vector2 operator*(const Vector2 vec) {
		return { x * vec.x, y * vec.y };
	}
	Vector2 operator+(const float amount) {
		return { x + amount, y + amount };
	}
	Vector2 operator+(const Vector2 vec) {
		return { x + vec.x, y + vec.y };
	}
	Vector2 operator-(const float amount) {
		return { x - amount, y - amount };
	}
	Vector2 operator-(const Vector2 vec) {
		return { x - vec.x, y - vec.y };
	}
};
#pragma pack(pop)
#pragma pack(push, 8)
class RVector3 {
public:
	RVector3() : x(0.f), y(0.f), z(0.f) {}
	RVector3(float x, float y, float z) : x(x), y(y), z(z) {}
public:
	alignas(sizeof(double)) float x, y, z;
public:
	RVector3 operator+(const RVector3& other) {
		RVector3 vec;
		vec.x = this->x + other.x;
		vec.y = this->y + other.y;
		vec.z = this->z + other.z;
		return vec;
	}
	RVector3 operator-(const RVector3& other) {
		RVector3 vec;
		vec.x = this->x - other.x;
		vec.y = this->y - other.y;
		vec.z = this->z - other.z;
		return vec;
	}
	RVector3 operator*(const RVector3& other) {
		RVector3 vec;
		vec.x = this->x * other.x;
		vec.y = this->y * other.y;
		vec.z = this->z * other.z;
		return vec;
	}
	RVector3 operator*(const float& other) {
		RVector3 vec;
		vec.x = this->x * other;
		vec.y = this->y * other;
		vec.z = this->z * other;
		return vec;
	}
	bool operator==(const RVector3& other)
	{
		return this->x == other.x && this->y == other.y && this->z == other.z;
	}

	bool operator!=(const RVector3& other)
	{
		return this->x != other.x || this->y != other.y || this->z != other.z;
	}
};
#pragma pack(pop)
#pragma pack(push, 8)
class Vector4 {
public:
	Vector4() = default;
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	alignas(sizeof(double)) float x;
	float y;
	float z;
	float w;
	Vector4 operator*(const float amount) {
		return { x * amount, y * amount, z * amount, w * amount };
	}
	Vector4 operator*(const Vector4& vec) {
		return { x * vec.x, y * vec.y, z * vec.z, w * vec.w };
	}
	Vector4 operator+(const float amount) {
		return { x + amount, y + amount, z + amount, w + amount };
	}
	Vector4 operator+(const Vector4& vec) {
		return { x + vec.x, y + vec.y, z + vec.z, w + vec.z };
	}
	Vector4 operator-(const float amount) {
		return { x - amount, y - amount, z - amount, w - amount };
	}
	Vector4 operator-(const Vector4& vec) {
		return { x - vec.x, y - vec.y, z - vec.z, w - vec.z };
	}
};
#pragma pack(pop)