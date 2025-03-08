#pragma once
#include "hooking.hpp"
namespace base
{
	void hooks::invalid_mods_crash_detour(int64_t a1, int64_t a2, int a3, char a4) {
		if (!*(int64_t*)(a1 + 0xD8))
			return;
		g_hooking->m_invalidModsCrashDetourHk.getOg<decltype(&invalid_mods_crash_detour)>()(a1, a2, a3, a4);
	}
	uint64_t hooks::invalid_decal(uintptr_t a1, int a2) {
		if (a1 && a2 == 2)
			//*(*(*(a1 + 0x48) + 0x30) + 0x2C8)
			if (const auto ptr = *reinterpret_cast<uintptr_t*>((a1 + 0x48)); ptr)
				if (const auto ptr2 = *reinterpret_cast<uintptr_t*>((ptr + 0x30)); ptr2)
					if (*reinterpret_cast<uintptr_t*>(ptr2 + 0x2C8) == 0)
						return 0;
		return g_hooking->m_invalidDecalHk.getOg<decltype(&invalid_decal)>()(a1, a2);
	}
	int hooks::task_parachute_object_0x270(uint64_t _this, int a2, int a3)
	{
		if (a2 == 1 && a3 == 1)// enter crash func
		{
			if (auto ptr = *(uint64_t*)(_this + 16))
				if (auto ptr2 = *(uint64_t*)(ptr + 80))
					if (auto ptr3 = *(uint64_t*)(ptr2 + 64))
						return g_hooking->m_taskParachuteObject0x270Hk.getOg<decltype(&task_parachute_object_0x270)>()(_this, a2, a3);
			return 0;
		}
		return g_hooking->m_taskParachuteObject0x270Hk.getOg<decltype(&task_parachute_object_0x270)>()(_this, a2, a3);
	}
	bool hooks::fragment_physics_crash_2(float* a1, float* a2) {
		if (!a1 || !a2)
			return false;
		return g_hooking->m_fragmentPhysicsCrash2Hk.getOg<decltype(&fragment_physics_crash_2)>()(a1, a2);
	}
}