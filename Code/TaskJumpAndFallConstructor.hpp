#pragma once
#include "Hooking.hpp"
#include "Features.hpp"
#include "HashList.hpp"
namespace Bits {
	template<typename T>
	constexpr void SetBit(T* value, T bit) {
		*value |= (1u << bit);
	}
	template<typename T>
	constexpr void ClearBit(T* value, T bit) {
		*value &= ~(1u << bit);
	}
	template<typename T>
	bool HasBitsSet(T* address, T bits) {
		return static_cast<bool>(*address & bits) && static_cast<bool>(*address == bits);
	}
	template<typename T, typename... Bits>
	constexpr void SetBits(T* value, Bits... bits);
	template<typename T, typename... Bits>
	constexpr void ClearBits(T* value, Bits... bits);;
	template<typename T, typename... Bits>
	constexpr void SetBits(T* value, Bits... bits) {
		(..., SetBit<T>(value, bits));
	}
	template<typename T, typename... Bits>
	constexpr void ClearBits(T* value, Bits... bits) {
		(..., ClearBit<T>(value, bits));
	}
}
namespace base {
	__int64 hooks::task_jump_constructor(std::uint64_t _This, int Flags) {
		switch (SuperJumpType) {
		case 1: {
			Flags |= (int)eTaskFlags::BeastJumpWithSuper;
			break;
		}
		case 2: {
			Flags |= (int)eTaskFlags::SuperJump;
			break;
		}
		default:
			break;
		}
		return g_hooking->m_taskJumpConstructorHk.getOg<decltype(&task_jump_constructor)>()(_This, Flags);
	}
	void* hooks::FallTaskConstructor(std::uint64_t _This, std::uint32_t Flags) {
		if (features::GracefulLanding) {
			Flags |= (uint32_t)eTaskFlags::GracefulLanding;
		}
		return g_hooking->m_taskFallConstructorHk.getOg<decltype(&FallTaskConstructor)>()(_This, Flags);
	}
}