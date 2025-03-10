#include "BytePatchMgr.hpp"
#include "GTAV-Classes-master/rage/atArray.hpp"
#include "Hooking.hpp"
#include "BytePatch.hpp"
#include "Pointers.hpp"
#include "KicksAndCrashes.hpp"
#include "NetArray.hpp"
extern "C" void sound_overload_detour();
std::uint64_t g_sound_overload_ret_addr;
namespace base
{
	static void init()
	{
		// Restore max wanted level after menu unload
		Toxic::police::m_max_wanted_level =
			memory::byte_patch::make(g_pointers->m_max_wanted_level.add(5).rip().as<uint32_t*>(), 0).get();
		Toxic::police::m_max_wanted_level_2 =
			memory::byte_patch::make(g_pointers->m_max_wanted_level.add(14).rip().as<uint32_t*>(), 0).get();

		// Patch blocked explosions
		Toxic::explosion_anti_cheat_bypass::m_can_blame_others =
			memory::byte_patch::make(g_pointers->m_blame_explode.as<std::uint16_t*>(), 0xE990).get();
		Toxic::explosion_anti_cheat_bypass::m_can_use_blocked_explosions =
			memory::byte_patch::make(g_pointers->m_explosion_patch.sub(12).as<uint16_t*>(), 0x9090).get();

		// Skip matchmaking session validity checks
		memory::byte_patch::make(g_pointers->m_is_matchmaking_session_valid.as<void*>(), std::to_array({ 0xB0, 0x01, 0xC3 }))->apply(); // has no observable side effects

		// Bypass netarray buffer cache when enabled
		broadcast_net_array::m_patch = memory::byte_patch::make(g_pointers->m_broadcast_patch.as<uint8_t*>(), 0xEB).get();

		// Disable cheat activated netevent when creator warping
		//memory::byte_patch::make(g_pointers->m_creator_warp_cheat_triggered_patch.as<uint8_t*>(), 0xEB)->apply();

		// PapiSysCallService VFT hook
		//memory::byte_patch::make(g_pointers->m_ntqvm_caller.add(4).rip().sub(32).as<uint64_t*>(), (uint64_t)&hooks::nt_query_virtual_memory)->apply();

		// Setup inline hook for sound overload crash protection
		g_sound_overload_ret_addr = g_pointers->m_sound_overload_detour.add(13 + 15).as<decltype(g_sound_overload_ret_addr)>();
		std::vector<byte> bytes = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90 }; // far jump opcode + a nop opcode
		*(void**)(bytes.data() + 6) = sound_overload_detour;
		memory::byte_patch::make(g_pointers->m_sound_overload_detour.add(13).as<void*>(), bytes)->apply();

		// Disable collision when enabled
		//vehicle::disable_collisions::m_patch =
			//memory::byte_patch::make(g_pointers->m_disable_collision.sub(2).as<uint8_t*>(), 0xEB).get();

		// Crash Trigger
		memory::byte_patch::make(g_pointers->m_crash_trigger.add(4).as<uint8_t*>(), 0x00)->apply();

		// Patch script network check
		memory::byte_patch::make(g_pointers->m_model_spawn_bypass, std::vector{ 0x90, 0x90 })->apply(); // this is no longer integrity checked

		// Prevent the game from crashing when flooded with outgoing events
		memory::byte_patch::make(g_pointers->m_free_event_error, std::vector{ 0x90, 0x90, 0x90, 0x90, 0x90 })->apply();

		// Script VM patches

		memory::byte_patch::make(g_pointers->m_script_vm_patch_1.add(2).as<uint32_t*>(), 0xc9310272)->apply();
		memory::byte_patch::make(g_pointers->m_script_vm_patch_1.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_script_vm_patch_2.add(2).as<uint32_t*>(), 0xc9310272)->apply();
		memory::byte_patch::make(g_pointers->m_script_vm_patch_2.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_script_vm_patch_3.add(2).as<uint32_t*>(), 0xd2310272)->apply();
		memory::byte_patch::make(g_pointers->m_script_vm_patch_3.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_script_vm_patch_4.add(2).as<uint32_t*>(), 0xd2310272)->apply();
		memory::byte_patch::make(g_pointers->m_script_vm_patch_4.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_script_vm_patch_5.add(2).as<uint32_t*>(), 0xd2310272)->apply();
		memory::byte_patch::make(g_pointers->m_script_vm_patch_5.add(6).as<uint16_t*>(), 0x9090)->apply();

		memory::byte_patch::make(g_pointers->m_script_vm_patch_6.add(2).as<uint32_t*>(), 0xd2310272)->apply();
		memory::byte_patch::make(g_pointers->m_script_vm_patch_6.add(6).as<uint16_t*>(), 0x9090)->apply();
	}

	byte_patch_manager::byte_patch_manager()
	{
		init();

		g_byte_patch_manager = this;
	}

	byte_patch_manager::~byte_patch_manager()
	{
		memory::byte_patch::restore_all();

		g_byte_patch_manager = nullptr;
	}
}