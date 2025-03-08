#pragma once
#include "NativeHooking.hpp"
#include "ScriptGlobal.hpp"
#include "Natives.hpp"
#include "Features.hpp"
#include "ui/dxUiManager.hpp"
#include "Enums.hpp"
#include "Translations.hpp"
#include <widemath.h>
#include "../GTAV-Classes-master/script/scrNativeHandler.hpp"
#include "Fibers/Fiber_pool.hpp"
#include "script_function.hpp"
#include "GtaUtill.hpp"
#include "Threading.hpp"
#include "Fibers/script_mgr.hpp"
#include "Services/Api/api_service.hpp"
namespace base
{
	namespace allscripts {
		namespace { std::uint32_t g_HookFrameCount{}; }
		void STAT_GET_INT(rage::scrNativeCallContext* src) {
			const auto statHash = src->get_arg<Hash>(0);
			const auto outValue = src->get_arg<int*>(1);
			const auto p2 = src->get_arg<int>(2);
			if (g_running && g_HookFrameCount != *g_pointers->m_frameCount) {
				g_HookFrameCount = *g_pointers->m_frameCount;
				if (callThreadCacheInsideANonScriptHook) {
					cacheThreadHandles();
					callThreadCacheInsideANonScriptHook = false;
				}
				g_script_mgr.tick();
			}
			src->set_return_value(STATS::STAT_GET_INT(statHash, outValue, p2));
		}
		void IS_DLC_PRESENT(rage::scrNativeCallContext* src) {
			const auto hash = src->get_arg<rage::joaat_t>(0);
			bool return_value = DLC::IS_DLC_PRESENT(hash);
			if (hash == 0x96F02EE6)
				return_value = return_value || features::dev_dlc;
			src->set_return_value<BOOL>((BOOL)return_value);
		}
		void RETURN_TRUE(rage::scrNativeCallContext* src) {
			src->set_return_value<BOOL>(TRUE);
		}
		void RETURN_FALSE(rage::scrNativeCallContext* src) {
			src->set_return_value<BOOL>(FALSE);
		}
		void DISABLE_CONTROL_ACTION(rage::scrNativeCallContext* src) {
			const auto action = src->get_arg<ControllerInputs>(1);
			if (features::interior_weapon) {
				switch (action) {
					// case ControllerInputs::INPUT_JUMP: TODO: add as separate feature
				case ControllerInputs::INPUT_ATTACK:
				case ControllerInputs::INPUT_AIM:
				case ControllerInputs::INPUT_DUCK:
				case ControllerInputs::INPUT_SELECT_WEAPON:
				case ControllerInputs::INPUT_COVER:
				case ControllerInputs::INPUT_TALK:
				case ControllerInputs::INPUT_DETONATE:
				case ControllerInputs::INPUT_WEAPON_SPECIAL:
				case ControllerInputs::INPUT_WEAPON_SPECIAL_TWO:
				case ControllerInputs::INPUT_VEH_AIM:
				case ControllerInputs::INPUT_VEH_ATTACK:
				case ControllerInputs::INPUT_VEH_ATTACK2:
				case ControllerInputs::INPUT_VEH_HEADLIGHT:
				case ControllerInputs::INPUT_VEH_NEXT_RADIO:
				case ControllerInputs::INPUT_VEH_PREV_RADIO:
				case ControllerInputs::INPUT_VEH_NEXT_RADIO_TRACK:
				case ControllerInputs::INPUT_VEH_PREV_RADIO_TRACK:
				case ControllerInputs::INPUT_VEH_RADIO_WHEEL:
				case ControllerInputs::INPUT_VEH_PASSENGER_AIM:
				case ControllerInputs::INPUT_VEH_PASSENGER_ATTACK:
				case ControllerInputs::INPUT_VEH_SELECT_NEXT_WEAPON:
				case ControllerInputs::INPUT_VEH_SELECT_PREV_WEAPON:
				case ControllerInputs::INPUT_VEH_ROOF:
				case ControllerInputs::INPUT_VEH_JUMP:
				case ControllerInputs::INPUT_VEH_FLY_ATTACK:
				case ControllerInputs::INPUT_MELEE_ATTACK_LIGHT:
				case ControllerInputs::INPUT_MELEE_ATTACK_HEAVY:
				case ControllerInputs::INPUT_MELEE_ATTACK_ALTERNATE:
				case ControllerInputs::INPUT_MELEE_BLOCK:
				case ControllerInputs::INPUT_SELECT_WEAPON_UNARMED:
				case ControllerInputs::INPUT_SELECT_WEAPON_MELEE:
				case ControllerInputs::INPUT_SELECT_WEAPON_HANDGUN:
				case ControllerInputs::INPUT_SELECT_WEAPON_SHOTGUN:
				case ControllerInputs::INPUT_SELECT_WEAPON_SMG:
				case ControllerInputs::INPUT_SELECT_WEAPON_AUTO_RIFLE:
				case ControllerInputs::INPUT_SELECT_WEAPON_SNIPER:
				case ControllerInputs::INPUT_SELECT_WEAPON_HEAVY:
				case ControllerInputs::INPUT_SELECT_WEAPON_SPECIAL:
				case ControllerInputs::INPUT_ATTACK2:
				case ControllerInputs::INPUT_MELEE_ATTACK1:
				case ControllerInputs::INPUT_MELEE_ATTACK2:
				case ControllerInputs::INPUT_VEH_GUN_LEFT:
				case ControllerInputs::INPUT_VEH_GUN_RIGHT:
				case ControllerInputs::INPUT_VEH_GUN_UP:
				case ControllerInputs::INPUT_VEH_GUN_DOWN:
				case ControllerInputs::INPUT_VEH_HYDRAULICS_CONTROL_TOGGLE:
				case ControllerInputs::INPUT_VEH_MELEE_HOLD:
				case ControllerInputs::INPUT_VEH_MELEE_LEFT:
				case ControllerInputs::INPUT_VEH_MELEE_RIGHT:
				case ControllerInputs::INPUT_VEH_CAR_JUMP:
				case ControllerInputs::INPUT_VEH_ROCKET_BOOST:
				case ControllerInputs::INPUT_VEH_FLY_BOOST:
				case ControllerInputs::INPUT_VEH_PARACHUTE:
				case ControllerInputs::INPUT_VEH_BIKE_WINGS:
				case ControllerInputs::INPUT_VEH_TRANSFORM: return;
				}
			}
			PAD::DISABLE_CONTROL_ACTION(src->get_arg<int>(0), (int)action, src->get_arg<int>(2));
		}
		void HUD_FORCE_WEAPON_WHEEL(rage::scrNativeCallContext* src) {
			if (features::interior_weapon && src->get_arg<BOOL>(0) == false)
				return;
			HUD::HUD_FORCE_WEAPON_WHEEL(src->get_arg<BOOL>(0));
		}
		void SET_CURRENT_PED_WEAPON(rage::scrNativeCallContext* src) {
			const auto ped = src->get_arg<Ped>(0);
			const auto hash = src->get_arg<rage::joaat_t>(1);
			if (features::interior_weapon && ped == PLAYER::PLAYER_PED_ID() && hash == "WEAPON_UNARMED"_joaat)
				return;
			WEAPON::SET_CURRENT_PED_WEAPON(ped, hash, src->get_arg<int>(2));
		}
		void DO_NOTHING(rage::scrNativeCallContext* src)
		{
		}
	}
	namespace freemode {
		void STAT_GET_INT(rage::scrNativeCallContext* src) {
			if (features::m_driver_performing_task && (src->get_arg<Hash>(0) == "MP0_PERSONAL_VEHICLE_ACCESS"_joaat || src->get_arg<Hash>(0) == "MP1_PERSONAL_VEHICLE_ACCESS"_joaat)) {
				src->set_return_value<int>(0);
				return;
			}
			src->set_return_value(STATS::STAT_GET_INT(src->get_arg<Hash>(0), src->get_arg<int*>(1), src->get_arg<int>(2)));
		}
		void IS_PLAYER_PLAYING(rage::scrNativeCallContext* src) {
			// block undead OTR
			if (features::decloak_players && src->get_arg<Player>(0) != PLAYER::PLAYER_ID() && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
				src->set_return_value<BOOL>(TRUE);
			else
				src->set_return_value<BOOL>(PLAYER::IS_PLAYER_PLAYING(src->get_arg<Player>(0)));
		};
		void SET_ENTITY_VISIBLE(rage::scrNativeCallContext* src) {
			auto entity = src->get_arg<Entity>(0);
			auto toggle = src->get_arg<bool>(1);
			auto outfit = src->get_arg<bool>(2);
			if (features::invisibilityBool && entity == PLAYER::PLAYER_PED_ID() && toggle)
				return;
			else
				ENTITY::SET_ENTITY_VISIBLE(entity, toggle, outfit);
		}
		void NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA(rage::scrNativeCallContext* src) {
			if (NETWORK::NETWORK_IS_ACTIVITY_SESSION() || NETWORK::NETWORK_IS_IN_TRANSITION() || NETWORK::NETWORK_IS_TRANSITION_TO_GAME() || NETWORK::NETWORK_IS_HOST_OF_THIS_SCRIPT()) {
				src->set_return_value<BOOL>(NETWORK::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA());
			}
			else {
				if (features::force_script_host && (features::fast_join || NETWORK::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA())) {
					auto hash = SCRIPT::GET_HASH_OF_THIS_SCRIPT_NAME();
					g_fiber_pool->queue_job([hash] {
						features::force_host(hash);
						if (auto script = gta_util::find_script_thread(hash); script && script->m_net_component)
							((rage::CGameScriptHandlerNetComponent*)script->m_net_component)->block_host_migration(true);
						});
				}
				if (SCRIPT::GET_HASH_OF_THIS_SCRIPT_NAME() == "freemode"_joaat && features::fast_join) {
					scr_functions::set_freemode_session_active({});
					src->set_return_value<BOOL>(TRUE);
				}
				else
				{
					src->set_return_value<BOOL>(NETWORK::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA());
				}
			}
		}
	}
	namespace shop_controller {
		void SET_WARNING_MESSAGE_WITH_HEADER(rage::scrNativeCallContext* src) {
			if (auto entry_line = src->get_arg<const char*>(1); strcmp(entry_line, "CTALERT_F_2") == 0) {
				MainNotification(ImGuiToastType_Warning, 4200, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("RECEIVED_TRANSACTION_LIMIT"));
				*globals(4536677).as<int*>() = 0;
				return;
			}
			HUD::SET_WARNING_MESSAGE_WITH_HEADER(src->get_arg<const char*>(0), src->get_arg<const char*>(1), src->get_arg<int>(2), src->get_arg<const char*>(3), src->get_arg<BOOL>(4), src->get_arg<Any>(5), src->get_arg<Any*>(6), src->get_arg<Any*>(7), src->get_arg<BOOL>(8), src->get_arg<Any>(9));
		}
	}
	namespace network {
		void NETWORK_SESSION_HOST(rage::scrNativeCallContext* src) {
			if (g_api_service->isQueued) {
				g_pointers->m_joinSessionByInfo(*g_pointers->m_network, &g_api_service->info, 1, 1 | 2, nullptr, 0);
				g_api_service->isQueued = false;
				src->set_return_value<BOOL>(TRUE);
			}
			else {
				src->set_return_value<BOOL>(NETWORK::NETWORK_SESSION_HOST(src->get_arg<int>(0), src->get_arg<int>(1), src->get_arg<BOOL>(2)));
			}
		}
		void NETWORK_BAIL(rage::scrNativeCallContext* src) {
			MainNotification(ImGuiToastType_Success, 5000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("BAIL_PREVENTED"), src->get_arg<int>(0), src->get_arg<int>(1), src->get_arg<int>(2), SCRIPT::GET_THIS_SCRIPT_NAME());
		}
		void NETWORK_CAN_BAIL(rage::scrNativeCallContext* src) {
			MainNotification(ImGuiToastType_Success, 5000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("NETWORK_BAIL_PREVENTED"));
			src->set_return_value<BOOL>(FALSE);
		}
	}
	namespace am_launcher {
		void START_NEW_SCRIPT_WITH_ARGS(rage::scrNativeCallContext* src) {
			const char* name = src->get_arg<const char*>(0);
			std::uint64_t* args = src->get_arg<std::uint64_t*>(1);
			int argc = src->get_arg<int>(2);
			int stackSize = src->get_arg<int>(3);
			if (Hash name_hash = rage::joaat(name); name_hash == "ggsm_arcade"_joaat || name_hash == "camhedz_arcade"_joaat || name_hash == "wizard_arcade"_joaat || name_hash == "puzzle"_joaat || name_hash == "fm_intro"_joaat || name_hash == "pilot_school_mp"_joaat || name_hash == "golf_mp"_joaat || name_hash == "tennis_network_mp"_joaat || name_hash == "fm_race_controler"_joaat || name_hash == "fm_horde_controler"_joaat || name_hash == "fm_mission_controller"_joaat || name_hash == "fm_mission_controller_2020"_joaat || name_hash == "fm_impromptu_dm_controler"_joaat || name_hash == "fm_deathmatch_controler"_joaat || name_hash == "fm_bj_race_controler"_joaat || name_hash == "fm_survival_controller"_joaat || name_hash == "tennis_network_mp"_joaat || name_hash == "sctv"_joaat || name_hash == "am_pi_menu"_joaat) {
				src->set_return_value<int>(0);
				return;
			}
			src->set_return_value<int>(SYSTEM::START_NEW_SCRIPT_WITH_ARGS(name, (Any*)args, argc, stackSize));
		};
	}
	namespace maintransition {
		void SC_TRANSITION_NEWS_SHOW(rage::scrNativeCallContext* src) {
			if (features::seamlessJoinBool)
				src->set_return_value<BOOL>(false);
			else
				src->set_return_value<BOOL>(SOCIALCLUB::SC_TRANSITION_NEWS_SHOW(src->get_arg<Any>(0)));
		}
		void SC_TRANSITION_NEWS_SHOW_TIMED(rage::scrNativeCallContext* src) {
			if (features::seamlessJoinBool)
				src->set_return_value<BOOL>(false);
			else
				src->set_return_value<BOOL>(SOCIALCLUB::SC_TRANSITION_NEWS_SHOW_TIMED(src->get_arg<Any>(0), src->get_arg<Any>(0)));
		}
		void IS_SWITCH_TO_MULTI_FIRSTPART_FINISHED(rage::scrNativeCallContext* src) {
			if (features::seamlessJoinBool)
				src->set_return_value<BOOL>(true);
			else
				src->set_return_value<BOOL>(STREAMING::IS_SWITCH_TO_MULTI_FIRSTPART_FINISHED());
		}
		void SET_FOCUS_ENTITY(rage::scrNativeCallContext* src) {
			if (features::seamlessJoinBool)
				STREAMING::SET_FOCUS_ENTITY(PLAYER::PLAYER_PED_ID());
			else
				STREAMING::SET_FOCUS_ENTITY(src->get_arg<Entity>(0));
		}
		void HIDE_HUD_AND_RADAR_THIS_FRAME(rage::scrNativeCallContext* src) {
			if (!features::seamlessJoinBool)
				HUD::HIDE_HUD_AND_RADAR_THIS_FRAME();
		}
		void ACTIVATE_FRONTEND_MENU(rage::scrNativeCallContext* src) {
			Hash menuhash = src->get_arg<Hash>(0);
			if (features::seamlessJoinBool && menuhash != "FE_MENU_VERSION_EMPTY_NO_BACKGROUND"_joaat)
				HUD::ACTIVATE_FRONTEND_MENU(menuhash, src->get_arg<BOOL>(1), src->get_arg<int>(2));
			if (!features::seamlessJoinBool)
				HUD::ACTIVATE_FRONTEND_MENU(menuhash, src->get_arg<BOOL>(1), src->get_arg<int>(2));
		}
		void RESTART_FRONTEND_MENU(rage::scrNativeCallContext* src) {
			Hash menuhash = src->get_arg<Hash>(0);
			if (features::seamlessJoinBool) {
				if (menuhash != "FE_MENU_VERSION_EMPTY_NO_BACKGROUND"_joaat) {
					HUD::RESTART_FRONTEND_MENU(menuhash, src->get_arg<int>(1));
				}
			}
			else {
				HUD::RESTART_FRONTEND_MENU(menuhash, src->get_arg<int>(1));
			}
		}
		void TOGGLE_PAUSED_RENDERPHASES(rage::scrNativeCallContext* src) {
			if (features::seamlessJoinBool)
				GRAPHICS::RESET_PAUSED_RENDERPHASES();
			else
				GRAPHICS::TOGGLE_PAUSED_RENDERPHASES(src->get_arg<int>(0));
		}
		void SET_ENTITY_VISIBLE(rage::scrNativeCallContext* src) {
			if (features::seamlessJoinBool && src->get_arg<Entity>(0) == PLAYER::PLAYER_PED_ID())
				ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), true, false);
			else
				ENTITY::SET_ENTITY_VISIBLE(src->get_arg<Entity>(0), src->get_arg<BOOL>(1), src->get_arg<BOOL>(2));
		}
		void SET_ENTITY_COORDS_NO_OFFSET(rage::scrNativeCallContext* src) {
			if (!features::seamlessJoinBool || *globals(1574993).as<eTransitionState*>() == eTransitionState::TRANSITION_STATE_CONFIRM_FM_SESSION_JOINING || src->get_arg<Entity>(0) != PLAYER::PLAYER_PED_ID())
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(src->get_arg<Entity>(0), Vector3(src->get_arg<float>(1), src->get_arg<float>(2), src->get_arg<float>(3)), src->get_arg<BOOL>(4), src->get_arg<BOOL>(5), src->get_arg<BOOL>(6));
		}
		void SET_ENTITY_COLLISION(rage::scrNativeCallContext* src) {
			if (!features::seamlessJoinBool || src->get_arg<Entity>(0) != PLAYER::PLAYER_PED_ID())
				ENTITY::SET_ENTITY_COLLISION(src->get_arg<Entity>(0), src->get_arg<BOOL>(1), src->get_arg<BOOL>(2));
		}
		void SET_PLAYER_CONTROL(rage::scrNativeCallContext* src) {
			if (!features::seamlessJoinBool)
				PLAYER::SET_PLAYER_CONTROL(src->get_arg<Player>(0), src->get_arg<BOOL>(1), src->get_arg<int>(2));
		}
		void FREEZE_ENTITY_POSITION(rage::scrNativeCallContext* src) {
			if (!features::seamlessJoinBool || src->get_arg<Entity>(0) != PLAYER::PLAYER_PED_ID())
				ENTITY::FREEZE_ENTITY_POSITION(src->get_arg<Entity>(0), src->get_arg<BOOL>(1));
		}
		void NETWORK_RESURRECT_LOCAL_PLAYER(rage::scrNativeCallContext* src) {
			if (!features::seamlessJoinBool)
				NETWORK::NETWORK_RESURRECT_LOCAL_PLAYER(Vector3(src->get_arg<float>(0), src->get_arg<float>(1), src->get_arg<float>(2)), src->get_arg<float>(3), src->get_arg<BOOL>(4), src->get_arg<BOOL>(5), src->get_arg<BOOL>(6), src->get_arg<int>(7), src->get_arg<int>(8));
		}
		void GET_EVER_HAD_BAD_PACK_ORDER(rage::scrNativeCallContext* src) {
			src->set_return_value<BOOL>(false);
		}
	}
}