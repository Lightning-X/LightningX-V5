#pragma once
#include "Pointers.hpp"
#include "Natives.hpp"
#include <random>
using namespace base;
namespace math
{
	inline float deg_to_rad(float deg) {
		double radian = (3.14159265359 / 180) * deg;
		return (float)radian;
	}
	inline float rad_to_deg(float rad) {
		double degree = (180 / 3.14159265359) * rad;
		return (float)degree;
	}
	inline float distance_between_vectors(Vector3 a, Vector3 b) {
		return (float)sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2) + pow((a.z - b.z), 2));
	}
	inline Vector3 rotation_to_direction(Vector3 rotation) {
		float x = deg_to_rad(rotation.x);
		float z = deg_to_rad(rotation.z);
		float num = abs(cos(x));
		return Vector3{ -sin(z) * num, cos(z) * num, sin(x) };
	}
	inline float calculate_distance_from_game_cam(rage::fvector3 player_position) {
		const Vector3 plyr_coords = { player_position.x, player_position.y, player_position.z };
		const Vector3 cam_coords = g_pointers->m_getGameplayCamCoords();
		return distance_between_vectors(plyr_coords, cam_coords);
	}
	inline Vector3 add_Vectors(Vector3 vector, Vector3 vector2) {
		vector.x += vector2.x;
		vector.y += vector2.y;
		vector.z += vector2.z;
		return vector;
	}
	inline Vector3 multiply_Vector(Vector3 vector, float inc) {
		vector.x *= inc;
		vector.y *= inc;
		vector.z *= inc;
		return vector;
	}
	class rng {
		std::random_device m_rd{};
		std::mt19937 m_mtRes{};
		size_t m_min;
		size_t m_max;
	public:
		template <typename t> requires std::is_integral_v<t>
		rng() : m_mtRes(m_rd()), m_min(std::numeric_limits<t>::min()), m_max(std::numeric_limits<t>::max()) {}
		rng(size_t min, size_t max) : m_mtRes(m_rd()), m_min(min), m_max(max) {}
		template <typename t>
		t get() {
			std::uniform_int_distribution<std::mt19937::result_type> m_intDis(m_min, m_max);
			return static_cast<t>(m_intDis(m_mtRes));
		}
	};
}