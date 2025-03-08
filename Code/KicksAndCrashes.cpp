#pragma once
#include "KicksAndCrashes.hpp"
#include "GTAV-Classes-master/network/snSession.hpp"
#include "GtaUtill.hpp"
#include "Features.hpp"
#include "Notify.hpp"
#include "Translations.hpp"
#include "Entity.hpp"
#include "Enums.hpp"
#include "GTAV-Classes-master/script/globals/GPBD_FM_3.hpp"
#include "HashList.hpp"
#include "Fibers/fiber_pool.hpp"
using namespace base;
namespace Toxic {
	void BreakUpKick(player_ptr player) {
		if (!g_player_service->get_self()->is_host() || !player->get_net_data())
			return;
		rage::snMsgRemoveGamersFromSessionCmd cmd{};
		cmd.m_session_id = gta_util::get_network()->m_game_session_ptr->m_rline_session.m_session_id;
		cmd.m_num_peers = 1;
		cmd.m_handles[0] = player->get_net_data()->m_gamer_handle;
		cmd.m_unk = 19;
		g_pointers->m_handleRemoveGamerCmd(gta_util::get_network()->m_game_session_ptr, player->get_session_player(), &cmd);
	}
	void HostKick(player_ptr player) {
		g_fiber_pool->queue_job([player] {
			if (!g_player_service->get_self()->is_host()) {
				MainNotification(ImGuiToastType_Info, 4200, TRANSLATE("HOST_KICK"), TRANSLATE("HOST_KICK_FAILED"));
				return;
			}
			NETWORK::NETWORK_SESSION_KICK_PLAYER(player->id());
			});
	}
	void DesyncKick(CNetGamePlayer* player) {
		g_fiber_pool->queue_job([player] {
			switch (DesyncKickType)
			{
			case 0: {
				if (gta_util::get_network()->m_game_session_ptr->is_host()) {
					gta_util::get_network()->m_game_complaint_mgr.raise_complaint(player->GetGamerInfo()->m_host_token);
					return;
				}
				g_player_service->m_player_to_use_complaint_kick = g_player_service->get_by_id(player->m_player_id);
				MainNotification(ImGuiToastType_Info, 4200, TRANSLATE("DESYNC"), TRANSLATE("DESYNC_KICK_SEND"));
			} break;
			case 1: {
				auto playerMgr = gta_util::get_network_player_mgr();
				if (playerMgr == nullptr) {
					return;
				}

				auto localPlayerID = playerMgr->m_local_net_player->m_player_id;

				if (player->m_player_id != localPlayerID && !player->IsNetworkHost()) {
					for (const auto& desyncWeapons : weaponHashes) {
						WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->m_player_id), desyncWeapons);
					}
					playerMgr->RemovePlayer(player);
					playerMgr->UpdatePlayerListsForPlayer(player);//maybe if we remove it they will be kicked faster???

					if (gta_util::get_network()->m_game_session.is_host()) {
						playerMgr->UpdatePlayerListsForPlayer(playerMgr->m_local_net_player);
					}
					else {
						for (const auto& SHost : playerMgr->m_player_list) {
							if (SHost && SHost->IsConnected() && SHost->IsNetworkHost()) {
								playerMgr->UpdatePlayerListsForPlayer(SHost);
							}
						}
					}
				}
				else {
					MainNotification(ImGuiToastType_Error, 5000, TRANSLATE("DESYNC"), TRANSLATE("FAILED_TO_DESYNC_DUE_TO_REASONS"));
				}
			} break;
			default:
				break;
			}
			});
	}

	void NullFunctionKick(player_ptr player) {
		const size_t arg_count = 0xF;
		int64_t args[arg_count] = { (int64_t)eScriptEvents::InteriorControl, (int64_t)PLAYER::PLAYER_ID(), (int64_t)-1 };
		g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->id());
	}
	void BreakFragmentCrash(player_ptr player) {
		auto Coords = *player->get_ped()->get_position();
		auto CrashModel = "prop_fragtest_cnst_04"_joaat;
		while (!STREAMING::HAS_MODEL_LOADED(CrashModel)) {
			STREAMING::REQUEST_MODEL(CrashModel);
			script::get_current()->yield(100ms);
		}
		static Object obj;
		gta_util::execute_as_script(*g_pointers->m_isSessionActive ? "freemode"_joaat : "main_persistent"_joaat, [CrashModel, Coords] {
			obj = OBJECT::CREATE_OBJECT_NO_OFFSET(CrashModel, Coords, true, false, false);
			});
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(CrashModel);
		OBJECT::BREAK_OBJECT_FRAGMENT_CHILD(obj, NULL, NULL);
		script::get_current()->yield(1s);
		if (take_control_of(obj)) {
			delete_entity(obj);
		}
	}
	void InvalidPedCrash(player_ptr player) {
		auto pos = *player->get_ped()->get_position();
		while (!STREAMING::HAS_MODEL_LOADED("slod_human"_joaat)) {
			STREAMING::REQUEST_MODEL("slod_human"_joaat);
			script::get_current()->yield();
		}
		static Ped crashPed;
		gta_util::execute_as_script(*g_pointers->m_isSessionActive ? "freemode"_joaat : "main_persistent"_joaat, [pos] {
			crashPed = PED::CREATE_PED(ePedType::PED_TYPE_MISSION, "slod_human"_joaat, pos, 0.f, TRUE, FALSE);
			});
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED("slod_human"_joaat);
		script::get_current()->yield(3000ms);
		if (take_control_of(crashPed))
			delete_entity(crashPed);
	}
	void ScriptHostKick(player_ptr player) {
		if (NETWORK::NETWORK_GET_HOST_OF_SCRIPT("freemode", -1, 0) != PLAYER::PLAYER_ID()) {
			features::force_host("freemode"_joaat);
		}
		script::get_current()->yield(350ms);
		*globals(1885447).at(player->id(), 1).as<bool*>() = true;
	}
	void BailKick(player_ptr player) {
		const size_t arg_count = 3;
		int64_t args[arg_count] = { (int64_t)eScriptEvents::NetworkBail,
			(int64_t)PLAYER::PLAYER_ID(),
			globals(1895156).as<GPBD_FM_3*>()->Entries[player->id()].ScriptEventReplayProtectionCounter };
		g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->id());
	}
	void EndSessionKick(player_ptr player) {
		g_fiber_pool->queue_job([=] {
			if (NETWORK::NETWORK_GET_HOST_OF_SCRIPT("freemode", -1, 0) != PLAYER::PLAYER_ID()) {
				features::force_host("freemode"_joaat);
			}
			script::get_current()->yield(350ms);
			g_player_service->m_player_to_use_end_session_kick = player;
			*globals(2648711).as<int*>() = (int)(__rdtsc() % 50000) + 6;//(int)(__rdtsc() % 50000) + 6; // making the game trigger the broadcast is a bit difficult and requires a little bit of tampering with the value and some luck
			});
	}
	void blame_explode_coord(player_ptr to_blame, Vector3 pos, eExplosionTag explosion_type, float damage, bool is_audible, bool is_invisible, float camera_shake)
	{
		explosion_anti_cheat_bypass::m_can_blame_others->apply();
		explosion_anti_cheat_bypass::m_can_use_blocked_explosions->apply();

		FIRE::ADD_OWNED_EXPLOSION(
			(*g_pointers->m_isSessionActive && to_blame) ? PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(to_blame->id()) : 0,
			pos,
			(int)explosion_type,
			damage,
			is_audible,
			is_invisible,
			camera_shake);

		explosion_anti_cheat_bypass::m_can_use_blocked_explosions->restore();
		explosion_anti_cheat_bypass::m_can_blame_others->restore();
	}
	void blame_explode_player(player_ptr to_blame, player_ptr target, eExplosionTag explosion_type, float damage, bool is_audible, bool is_invisible, float camera_shake)
	{
		Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(target->id()), true);
		blame_explode_coord(to_blame, coords, explosion_type, damage, is_audible, is_invisible, camera_shake);
	}
}