#pragma once
#include "Pointers.hpp"
#include "Services/ScriptPatcher/script_patcher_service.hpp"
#include "GTAV-Classes-master/script/scrProgramTable.hpp"
#include "joaat2.hpp"
#include "Features.hpp"
namespace base {
	void register_script_patches() {
		//g_script_patcher_service->add_patch(
		//	{ "freemode"_joaat,, "2D 01 08 00 ? 38 00 5D ? ? ? 2A 06", 5, {0x71, 0x2E, 0x01, 0x01}, &g.session.decloak_players });
		g_script_patcher_service->add_patch({ "freemode"_joaat, "2D 01 04 00 ? 2C ? ? ? 5D ? ? ? 71 57 ? ? 2C", 5, {0x2E, 0x01, 0x00}, nullptr }); // script host kick
		g_script_patcher_service->add_patch({ "freemode"_joaat, "5D ? ? ? 76 57 ? ? 5D ? ? ? 76", 0, {0x2E, 0x00, 0x00}, nullptr }); // end session kick protection
		g_script_patcher_service->add_patch({ "freemode"_joaat, "2D 01 09 00 00 5D ? ? ? 56 ? ? 2E", 5, {0x2E, 0x01, 0x00}, nullptr }); // disable death when undermap/spectating
		g_script_patcher_service->add_patch({ "freemode"_joaat, "71 2E ? ? 55 ? ? 61 ? ? ? 47 ? ? 63", 0, {0x72}, nullptr }); // load island even if stranded animal IPL choice is not set
		g_script_patcher_service->add_patch({ "freemode"_joaat, "2D 00 07 00 00 7B", 5, {0x2E, 0x00, 0x00}, nullptr }); // disable population load balancing
		g_script_patcher_service->add_patch({ "freemode"_joaat, "5D ? ? ? 56 ? ? 72 39 05 38 04 2C ? ? ? 58", 0, {0x2B, 0x2B, 0x2B, 0x00, 0x55}, &features::invisibilityBool });
		g_script_patcher_service->add_patch({ "freemode"_joaat, "2D 01 03 00 00 38 00 71 72 5D ? ? ? 06 56 ? ? 71 2E ? ? 2C ? ? ? 71", 5, {0x72, 0x2E, 0x01, 0x01}, &features::unhide_players_from_player_list }); // unhide players from player list
		//g_script_patcher_service->add_patch(
		//	{ "freemode"_joaat,, "2D 02 08 00 00 38 01 56", 5, {0x2E, 0x02, 0x00}, &g.session.block_muggers });
		//g_script_patcher_service->add_patch({ "freemode"_joaat,, "2D 00 CF 00 00", 5, {0x2E, 0x00, 0x00}, &g.session.block_ceo_raids });
		//g_script_patcher_service->add_patch(
		//	{ "freemode"_joaat,, "06 56 ? ? 38 02 2C ? ? ? 71 71", 0, {0x2B, 0x55}, &g.spoofing.spoof_blip }); // prevent normal blip update
		//g_script_patcher_service->add_patch(
		//	{ "freemode"_joaat,, "2C ? ? ? 55 ? ? 71 2C ? ? ? 61", 7, std::vector<uint8_t>(16, 0x0), &g.spoofing.spoof_blip }); // prevent normal blip update 2
		g_script_patcher_service->add_patch({ "shop_controller"_joaat, "2D 01 04 00 00 2C ? ? ? 56 ? ? 71", 5, {0x71, 0x2E, 0x01, 0x01}, nullptr }); // despawn bypass
		g_script_patcher_service->add_patch({ "shop_controller"_joaat, "38 00 5D ? ? ? 38 00 5D ? ? ? 38 00 41", 0, std::vector<uint8_t>(12, 0x0), nullptr }); // godmode/invisibility detection bypass
		for (auto& entry : *g_pointers->m_scriptProgramTable) {
			if (entry.m_program)
				g_script_patcher_service->on_script_load(entry.m_program);
		}
	}
	void ScriptPatchLoop() {
		register_script_patches();
	}
}