#include "Features.hpp"
#include "ui/dxUiManager.hpp"
#include "HTTPRequest.hpp"
#include "JSON.hpp"
#include "GtaUtill.hpp"
#include "Enums.hpp"
#include "SpinnerService.hpp"
#include "HashList.hpp"
#include "Fibers/script.hpp"
#include "Fibers/fiber_pool.hpp"
#include "Entity.hpp"
#include "NoClip.hpp"
#include "FreeCam.hpp"
#include "Labels.hpp"
#include "Services/Player/player_service.hpp"
#include "GTAV-Classes-master/script/globals/GlobalPlayerBD.hpp"
#include "GTAV-Classes-master/base/phBoundComposite.hpp"
#include "Hooking.hpp"
#include "Math.hpp"
#include "KicksAndCrashes.hpp"
#include "script_function.hpp"
namespace base::features {
	const auto playerGlobal = globals(2657704);
	bool sessionJoin(eSessionType session) {
		SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH("pausemenu_multiplayer"_joaat);
		while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED("pausemenu_multiplayer"_joaat))
			script::get_current()->yield();
		*globals(2695915).as<int*>() = (session == eSessionType::SC_TV ? 1 : 0); // If SCTV then enable spectator mode
		if (session == eSessionType::LEAVE_ONLINE)
			*globals(1574589).at(2).as<int*>() = -1;
		else {
			*globals(1574589).at(2).as<int*>() = 0;
			*globals(1575020).as<int*>() = (int)session;
		}
		*globals(1574589).as<int*>() = 1;
		if (*g_pointers->m_isSessionActive && session != eSessionType::LEAVE_ONLINE) {
			*globals(1574996).as<eTransitionState*>() = eTransitionState::TRANSITION_STATE_LOOK_TO_JOIN_ANOTHER_SESSION_FM;
		}
		else if (session == eSessionType::LEAVE_ONLINE) {
			*globals(1574996).as<eTransitionState*>() = eTransitionState::TRANSITION_STATE_RETURN_TO_SINGLEPLAYER;
		}
		scr_functions::reset_session_data({});
		*globals(32284).as<int*>() = 0;
		*globals(1574934).as<int*>() = 1;
		*globals(1574995).as<int*>() = 32;
		if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) == 0) {
			*globals(2694534).as<int*>() = 1;
			script::get_current()->yield(200ms);
			*globals(1574589).as<int*>() = 0;
		}
		SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED("pausemenu_multiplayer"_joaat);
		return true;
	}
	void doAnimation(const char* anim, const char* animid) {
		g_fiber_pool->queue_job([anim, animid] {
			int pPlayer = PLAYER::PLAYER_PED_ID();
			STREAMING::REQUEST_ANIM_DICT(anim);
			while (!STREAMING::HAS_ANIM_DICT_LOADED((anim))) {
				STREAMING::REQUEST_ANIM_DICT(anim);
				script::get_current()->yield(10ms);
			}
			TASK::TASK_PLAY_ANIM(pPlayer, anim, animid, 8.0f, 0.0f, -1, eAnimationFlags::ANIM_FLAG_ENABLE_PLAYER_CONTROL, 0, 0, 0, 0);
			});
	}
	bool force_host(uint32_t hash) {
		if (auto launcher = gta_util::find_script_thread(hash); launcher && launcher->m_net_component) {
			for (int i = 0; !((rage::CGameScriptHandlerNetComponent*)launcher->m_net_component)->is_local_player_host(); i++) {
				if (i > 200)
					return false;
				((rage::CGameScriptHandlerNetComponent*)launcher->m_net_component)
					->send_host_migration_event(g_player_service->get_self()->get_net_game_player());
				script::get_current()->yield(10ms);
				if (!launcher->m_stack || !launcher->m_net_component)
					return false;
			}
		}
		return true;
	}
	float GetWeaponDamage(rage::joaat_t weapon) {
		return WEAPON::GET_WEAPON_DAMAGE(weapon, 0);
	}
	void applyChosenSkin(const Hash model) {
		g_fiber_pool->queue_job([&model] {
			STREAMING::REQUEST_MODEL(model);
			for (uint8_t i{}; !STREAMING::HAS_MODEL_LOADED(model) && i < 50; i++) {
				STREAMING::REQUEST_MODEL(model);
				script::get_current()->yield();
			}
			if (!STREAMING::HAS_MODEL_LOADED(model)) {
				MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("SKIN_CHANGER"), TRANSLATE("FAILED_TO_FIND_MODEL"));
				return;
			}
			if (!STREAMING::IS_MODEL_A_PED(model) ||
				model == "slod_human"_joaat || model == "slod_small_quadped"_joaat || model == "slod_large_quadped"_joaat)
			{
				MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("SKIN_CHANGER"), TRANSLATE("IS_NOT_A_PED"), model);
				return;
			}
			PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model);
			script::get_current()->yield();
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
			});
	}
	void RGBFadeInt() {
		if (redcolorInt > 0 && bluecolorInt == 0) {
			--redcolorInt;
			++greencolorInt;
		}
		else if (greencolorInt > 0 && redcolorInt == 0) {
			--greencolorInt;
			++bluecolorInt;
		}
		else if (bluecolorInt > 0 && greencolorInt == 0) {
			++redcolorInt;
			--bluecolorInt;
		}
	}
	std::array<std::string, 35> plateSymbols{ "A", "B", "C", "D", "E", "F",
		"G", "H", "I", "J", "K", "L",
		"M", "N", "O", "P", "Q",
		"R", "S", "T", "U", "V",
		"W", "X", "Y", "Z", "1",
		"2", "3", "4", "5", "6", "7",
		"8", "9", };
	//Features
	static bool bLastGodmode = false;
	void godMode() {
		bool bGodmode = godModeBool;
		if (bGodmode || (!bGodmode && bGodmode != bLastGodmode)) {
			gta_util::execute_as_script("main_persistent"_joaat, [] {
				ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), godModeBool);
				});
			bLastGodmode = bGodmode;
		}
	}
	static bool BLastInvis = false;
	void invisibility() {
		bool bInvis = invisibilityBool;
		if (bInvis || (!bInvis && bInvis != BLastInvis)) {
			ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), invisibilityBool ? (!*g_pointers->m_isSessionActive && locallyVisbleBool ? true : false) : true, FALSE);
			if (locallyVisbleBool)
				NETWORK::SET_LOCAL_PLAYER_VISIBLE_LOCALLY(true);
			locallyVisbleBool = !InvisibleState;
			BLastInvis = invisibilityBool;
		}
	}
	bool bLastOffRadarBool = false;
	void offRadar() {
		bool bOffRadar = offRadarBool;
		if (bOffRadar || (bOffRadar != bLastOffRadarBool)) {
			GlobalPlayerBD* pGlobalPlayerBD = globals(playerGlobal).as<GlobalPlayerBD*>();
			pGlobalPlayerBD->Entries[PLAYER::PLAYER_ID()].OffRadarActive = offRadarBool;
			int* pNetworkTime = globals(2672524).at(57).as<int*>();
			*pNetworkTime = NETWORK::GET_NETWORK_TIME() + (offRadarBool ? 0xB8E10 : 0);
			bLastOffRadarBool = offRadarBool;
		}
	}
	void fastSwim() {
		if (fastSwimSpeed != 1.f) {
			PED::SET_PED_MOVE_RATE_IN_WATER_OVERRIDE(PLAYER::PLAYER_PED_ID(), fastSwimSpeed);
		}
	}
	void fastRun() {
		if (fastRunSpeed != 1.f) {
			PED::SET_PED_MOVE_RATE_OVERRIDE(PLAYER::PLAYER_PED_ID(), fastRunSpeed);
		}
	}
	void neverWanted() {
		if (gta_util::get_local_ped() == nullptr || gta_util::get_local_ped()->m_player_info == nullptr)
			return;
		static bool bLast = false;
		bool b = neverWantedBool;
		if (b) {
			gta_util::get_local_ped()->m_player_info->m_wanted_level = 0;
			Toxic::police::m_max_wanted_level->apply();
			Toxic::police::m_max_wanted_level_2->apply();
			bLast = b;
		}
		else if (b != bLast) {
			Toxic::police::m_max_wanted_level->restore();
			Toxic::police::m_max_wanted_level_2->restore();
			bLast = b;
		}
	}
	static bool bLastNoRagdoll = false;
	void noRagdoll() {
		bool bNoRagdoll = noRagdollBool;
		if (bNoRagdoll != bLastNoRagdoll) {
			PED::SET_PED_CAN_RAGDOLL(PLAYER::PLAYER_PED_ID(), !bNoRagdoll);
			bLastNoRagdoll = bNoRagdoll;
		}
	}
	void superRun() {
		if (superRunBool && PAD::IS_DISABLED_CONTROL_PRESSED(2, 32) && PAD::IS_DISABLED_CONTROL_PRESSED(2, 21)) {
			ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, Vector3(0.0f, 1.3f, 0.f), Vector3(0.f, 0.f, 0.f), false, true, true, true, false, false);
		}
	}
	static bool bLastSwimOnAir = false;
	void swimInAir() {
		bool bSwimOnAir = swimInAirBool;
		bool bPedSwimming = PED::IS_PED_SWIMMING(PLAYER::PLAYER_PED_ID());
		if (bSwimOnAir != bLastSwimOnAir || (bSwimOnAir && !bPedSwimming)) {
			PED::SET_PED_CONFIG_FLAG(PLAYER::PLAYER_PED_ID(), 65, bSwimOnAir);
			bLastSwimOnAir = bSwimOnAir;
		}
	}
	void selfFastRespawn() {
		if (FastRespawnBool && PED::IS_PED_DEAD_OR_DYING(PLAYER::PLAYER_PED_ID(), FALSE)) {
			auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE);
			if (*g_pointers->m_isSessionActive)
				NETWORK::NETWORK_RESURRECT_LOCAL_PLAYER(pos, 0.f, FALSE, FALSE, FALSE, -1, -1);
			else {
				PED::RESURRECT_PED(PLAYER::PLAYER_PED_ID());
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(PLAYER::PLAYER_PED_ID(), pos, FALSE, FALSE, FALSE);
			}
			MISC::FORCE_GAME_STATE_PLAYING();
			TASK::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::PLAYER_PED_ID());
			MISC::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("respawn_controller");
		}
	}
	static bool bLastHealthRegen = false;
	void healthRegen() {
		bool bHealthRegen = healthRegenbool;
		if (bHealthRegen || (!bHealthRegen && bHealthRegen != bLastHealthRegen)) {
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(PLAYER::PLAYER_ID(), healthRegenSpeed);
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MAX_PERCENT(PLAYER::PLAYER_ID(), 99999.0f);
			if (!bHealthRegen) {
				PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(PLAYER::PLAYER_ID(), 1.0f);
				PLAYER::SET_PLAYER_HEALTH_RECHARGE_MAX_PERCENT(PLAYER::PLAYER_ID(), 1.0f);
			}
			bLastHealthRegen = bHealthRegen;
		}
	}
	void unlimitedOxygen() {
		if (unlimitedOxygenBool)
			gta_util::get_local_ped()->m_ped_intelligence->m_oxygen_time = 0;
	}
	void selfClean() {
		if (keepSelfCleanBool) {
			PED::CLEAR_PED_BLOOD_DAMAGE(PLAYER::PLAYER_PED_ID());
			PED::CLEAR_PED_WETNESS(PLAYER::PLAYER_PED_ID());
			PED::CLEAR_PED_ENV_DIRT(PLAYER::PLAYER_PED_ID());
			PED::RESET_PED_VISIBLE_DAMAGE(PLAYER::PLAYER_PED_ID());
		}
	}
	static bool bLastMobileRadio = false;
	void mobileRadio() {
		bool bMobileRadio = mobileRadioBool;
		if (bMobileRadio || (!bMobileRadio && bMobileRadio != bLastMobileRadio)) {
			AUDIO::SET_MOBILE_PHONE_RADIO_STATE(1);
			AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(1);
			if (!bMobileRadio) {
				AUDIO::SET_MOBILE_PHONE_RADIO_STATE(0);
				AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(0);
			}
			bLastMobileRadio = bMobileRadio;
		}
	}
	static bool bLastNoCollision = false;
	void noCollision() {
		bool bNoCollision = selfNoCollision;
		if (bNoCollision || (!bNoCollision && bNoCollision != bLastNoCollision)) {
			auto cped = (*g_pointers->m_pedFactory)->m_local_ped;
			((rage::phBoundComposite*)cped->m_navigation->m_damp->m_bound)->m_bounds[0]->m_bounding_box_max_xyz_margin_w.w = -1;
			if (!bNoCollision) {
				((rage::phBoundComposite*)cped->m_navigation->m_damp->m_bound)->m_bounds[0]->m_bounding_box_max_xyz_margin_w.w = 0.25;
			}
			bLastNoCollision = bNoCollision;
		}
	}
	static bool bLastWaterNoCollision = false;
	void noWaterCollision() {
		bool bNoWaterCollision = selfNoWaterCollision;
		if (bNoWaterCollision || (!bNoWaterCollision && bNoWaterCollision != bLastWaterNoCollision)) {
			auto cped = (*g_pointers->m_pedFactory)->m_local_ped;
			cped->m_navigation->m_damp->m_water_collision = 0;
			if (!bNoWaterCollision) {
				cped->m_navigation->m_damp->m_water_collision = 1;
			}
			bLastWaterNoCollision = bNoWaterCollision;
		}
	}
	//NETWORK
	static bool bReset = true;
	void spectatePlayer() {
		auto plyrMgr = gta_util::get_network_player_mgr()->m_player_list[g_selectedPlayer];
		const auto ped = PLAYER::PLAYER_PED_ID();
		if (plyrMgr == nullptr || !plyrMgr->IsConnected() || !SpectateBool) {
			if (SpectateBool) {
				SpectateBool = false;
			}
			if (!bReset) {
				bReset = true;
				NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(FALSE, -1);
				NETWORK::NETWORK_OVERRIDE_RECEIVE_RESTRICTIONS_ALL(FALSE);
				HUD::SET_MINIMAP_IN_SPECTATOR_MODE(FALSE, -1);
				HUD::SET_BLIP_ALPHA(HUD::GET_MAIN_PLAYER_BLIP_ID(), 255);
				STREAMING::SET_FOCUS_ENTITY(ped);
			}
			return;
		}
		const auto target = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(plyrMgr->m_player_id);
		NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(TRUE, target);
		NETWORK::NETWORK_OVERRIDE_RECEIVE_RESTRICTIONS_ALL(FALSE);
		HUD::SET_MINIMAP_IN_SPECTATOR_MODE(TRUE, target);
		HUD::SET_BLIP_ALPHA(HUD::GET_MAIN_PLAYER_BLIP_ID(), 255);
		STREAMING::SET_FOCUS_ENTITY(target);
		bReset = false;
	}
	static bool bLastForceHost = false;
	void forceSessionHost() {
		if (bLastForceHost != force_session_host && gta_util::get_network()->m_game_session_state == 0) {
			std::uint64_t host_token = force_session_host ? (rand() % 10000) : 0;
			if (force_session_host) {
				g_pointers->m_generateUuid(&host_token);
			}
			*g_pointers->m_host_token = host_token;
			if (auto game_session_ptr = gta_util::get_network()->m_game_session_ptr) {
				game_session_ptr->m_local_player.m_player_data.m_host_token = host_token;
			}
			g_pointers->m_profile_gamer_info->m_host_token = host_token;
			g_pointers->m_player_info_gamer_info->m_host_token = host_token;
			if (auto voice_connection = (*g_pointers->m_communications)->m_voice.m_connections[0]) {
				voice_connection->m_gamer_info.m_host_token = host_token;
			}
			if (auto local_ped = (*g_pointers->m_pedFactory)->m_local_ped; local_ped && local_ped->m_player_info) {
				local_ped->m_player_info->m_net_player_data.m_host_token = host_token;
			}
			bLastForceHost = force_session_host;
		}
	}
	static float old_percent = NAN;
	eTransitionState last_state = eTransitionState::TRANSITION_STATE_EMPTY;
	void hudTransitionState() {
		const auto state = *globals(1574996).as<eTransitionState*>();
		// When freemode script loaded remove loading screen.
		if (state == eTransitionState::TRANSITION_STATE_WAIT_JOIN_FM_SESSION && DLC::GET_IS_LOADING_SCREEN_ACTIVE())
		{
			SCRIPT::SHUTDOWN_LOADING_SCREEN();
		}

		if ((last_state == state && state != eTransitionState::TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER) || state == eTransitionState::TRANSITION_STATE_EMPTY || state > eTransitionState::TRANSITION_STATE_SPAWN_INTO_PERSONAL_VEHICLE)
		{
			return;
		}

		if (state != eTransitionState::TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER && HUD::BUSYSPINNER_IS_ON())
		{
			HUD::BUSYSPINNER_OFF();
		}
		if (g_spinner_service->spinner.active)
			g_spinner_service->spinner.active = false;

		if (state == eTransitionState::TRANSITION_STATE_TERMINATE_MAINTRANSITION)
			return;

		if ((int)state > 0 && (int)state < std::size(transition_states))
		{
			rage::scrThread* freemode{};
			if (state == eTransitionState::TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER
				&& (freemode = gta_util::find_script_thread("freemode"_joaat), freemode && freemode->m_net_component))
			{
				int num_array_handlers{};
				int received_array_handlers{};

				while (
					auto handler = g_pointers->m_get_host_array_handler_by_index((rage::CGameScriptHandlerNetComponent*)freemode->m_net_component, num_array_handlers++))
					if (handler->m_flags & 1)
						received_array_handlers++;

				if (num_array_handlers == 0)
					num_array_handlers = 1;

				float percent = round((static_cast<float>(received_array_handlers) / num_array_handlers) * 100);

				if (percent != old_percent)
				{
					auto spinner_text = std::format("Wait For Host Broadcast Data: {}%%", percent);
					g_spinner_service->push_online(true, spinner_text, true, percent);
				}

				old_percent = percent;
			}
			else
			{
				old_percent = NAN;
				auto spinner_text = std::format("{}", transition_states[(int)state]);
				bool show_percent = state > eTransitionState::TRANSITION_STATE_SP_SWOOP_UP || state < eTransitionState::TRANSITION_STATE_IS_FM_AND_TRANSITION_READY;
				float load_percent = (float)state / 27.f;
				g_spinner_service->push_online(true, spinner_text, show_percent, load_percent);
			}
		}

		last_state = state;
	}
	void seamlessJoin() {
		if (seamlessJoinBool && *globals(1574996).as<eTransitionState*>() <= eTransitionState::TRANSITION_STATE_FM_FINAL_SETUP_PLAYER && STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS()) {
			PLAYER::SET_PLAYER_CONTROL(PLAYER::PLAYER_PED_ID(), true, 0);
			STREAMING::STOP_PLAYER_SWITCH();
			GRAPHICS::ANIMPOSTFX_STOP_ALL(); // fix Sky cam effect
		}
	}
	//Weapons
	static bool bLastINfAmmo = false;
	void infiniteAmmo() {
		bool bINFAmmo = infiniteAmmoBool;
		if (bINFAmmo || (!bINFAmmo && bINFAmmo != bLastINfAmmo)) {
			WEAPON::SET_PED_INFINITE_AMMO_CLIP(PLAYER::PLAYER_PED_ID(), infiniteAmmoBool);
			bLastINfAmmo = bINFAmmo;
		}
	}
	void rapidFire() {
		if (Rapidfirebool && !HUD::IS_PAUSE_MENU_ACTIVE() && !PED::IS_PED_DEAD_OR_DYING(PLAYER::PLAYER_PED_ID(), true)) {
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK)) {
				const auto weapon_entity = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(PLAYER::PLAYER_PED_ID(), 0);
				if (!weapon_entity)
					return;
				Vector3 dim_min;
				Vector3 dim_max;
				MISC::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(weapon_entity), &dim_min, &dim_max);
				const auto dimensions = (dim_max - dim_min) * 0.5f;
				const auto weapon_position = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(weapon_entity, Vector3(dimensions.x, 0, dimensions.z * 0.3f));
				auto camera_direction = math::rotation_to_direction(CAM::GET_GAMEPLAY_CAM_ROT(0));
				auto camera_position = CAM::GET_GAMEPLAY_CAM_COORD() + camera_direction;
				Vector3 end = camera_position + camera_direction * 2000.0;
				const auto raycast_handle =
					SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(
						camera_position,
						end,
						-1, 0, 7);
				int did_raycast_hit = 0;
				Vector3 raycast_hit_position{};
				Vector3 raycast_surface_normal_hit_position{};
				Entity raycast_hit_entity{};
				SHAPETEST::GET_SHAPE_TEST_RESULT(raycast_handle, &did_raycast_hit, &raycast_hit_position, &raycast_surface_normal_hit_position, &raycast_hit_entity);
				if (did_raycast_hit)
				{
					end = raycast_hit_position;
				}
				Hash weapon_hash;
				WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &weapon_hash, false);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(weapon_position, end, WEAPON::GET_WEAPON_DAMAGE(weapon_hash, 0), true, weapon_hash, PLAYER::PLAYER_PED_ID(), true, false, -1.0);
			}
		}
	}
	Vector3 TpCoords;
	void Teleportgun() {
		if (Teleportgungun && WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &TpCoords)) {
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), FALSE) ? PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()) : PLAYER::PLAYER_PED_ID(), TpCoords, FALSE, FALSE, FALSE);
		}
	}
	const std::vector<rage::joaat_t> m_RPModelList = {
		"vw_prop_vw_colle_alien"_joaat,
		"vw_prop_vw_colle_imporage"_joaat,
		"vw_prop_vw_colle_beast"_joaat,
		"vw_prop_vw_colle_pogo"_joaat,
		"vw_prop_vw_colle_prbubble"_joaat,
		"vw_prop_vw_colle_rsrcomm"_joaat,
		"vw_prop_vw_colle_rsrgeneric"_joaat,
		"vw_prop_vw_colle_sasquatch"_joaat,
		"vw_prop_vw_lux_card_01a"_joaat,
	};
	static bool bLastRpGun = false;
	void RPGun() {
		bool bRpGun = RpGunbool;
		if (bRpGun || (!bRpGun && bRpGun != bLastRpGun)) {
			Hash CustomPUHash = "pickup_portable_crate_unfixed_incar_small"_joaat;
			Vector3 explosivecoords;
			for (auto const& RPModel : m_RPModelList) {
				while (!STREAMING::HAS_MODEL_LOADED(RPModel)) {
					STREAMING::REQUEST_MODEL(RPModel);
					script::get_current()->yield();
				}
				if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &explosivecoords)) {
					OBJECT::CREATE_AMBIENT_PICKUP(CustomPUHash, explosivecoords, 0, math::rng(0, 69).get<int>(), RPModel, false, true);
				}
				if (!bRpGun) {
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(RPModel);
				}
			}
			bLastRpGun = bRpGun;
		}
	}
	void Stealgun() {
		if (Stealgunbool && PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM)) {
			if (PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, (int)ControllerInputs::INPUT_ATTACK)) {
				Entity ent;
				if (raycast(&ent)) {
					if (ENTITY::IS_ENTITY_A_VEHICLE(ent)) {
						for (size_t i = 0; i < 8 && !VEHICLE::IS_VEHICLE_SEAT_FREE(ent, -1, 0); i++) {
							const auto ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(ent, -1, 0);
							if (PED::IS_PED_A_PLAYER(ped)) {
								g_pointers->m_clearPedTasksNetwork(gta_util::get_network_player_mgr()->m_player_list[PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(ped)]->m_player_info->m_ped, true);//not sure if it works -_-
							}
							else {
								TASK::CLEAR_PED_TASKS_IMMEDIATELY(ped);
								TASK::CLEAR_PED_SECONDARY_TASK(ped);
								TASK::CLEAR_PED_TASKS(ped);
							}
							script::get_current()->yield(100ms);
						}
						PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), ent, -1);
					}
				}
			}
		}
	}
	void MCgun() {
		if (Mcgunbool) {
			std::array<float, 6> impactCoords;
			WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), reinterpret_cast<Vector3*>(impactCoords.data()));
			if (std::any_of(impactCoords.begin(), impactCoords.end(), [](float coord) { return coord != 0.0f; })) {
				auto impactObject = OBJECT::CREATE_OBJECT(4146332269, Vector3(impactCoords[0], impactCoords[2], impactCoords[4]), 1, 0, 0);
				ENTITY::FREEZE_ENTITY_POSITION(impactObject, 1);
			}
		}
	}
	static bool bLastPTFXGun = false;
	void PTFXgun() {
		bool bPTFXGun = Coolptfxgun;
		if (bPTFXGun || (!bPTFXGun && bPTFXGun != bLastPTFXGun)) {
			while (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("scr_powerplay")) {
				STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_powerplay");
				script::get_current()->yield();
			}
			Vector3 v0;
			Vector3 v1;
			auto weaponIndexed = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(PLAYER::PLAYER_PED_ID(), 0);
			MISC::GET_MODEL_DIMENSIONS(WEAPON::GET_SELECTED_PED_WEAPON(PLAYER::PLAYER_PED_ID()), &v0, &v1);
			GRAPHICS::USE_PARTICLE_FX_ASSET("scr_powerplay");
			GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_PED_BONE("sp_powerplay_beast_appear_trails", weaponIndexed, Vector3((v0.x - v1.x) / 2.0f + 0.7f, 0.f, 0.f), Vector3(0.f, 180.f, 0.f), 1, 1, 1, 1, 1);
			if (!bPTFXGun)
				STREAMING::REMOVE_NAMED_PTFX_ASSET("scr_powerplay");
			bLastPTFXGun = bPTFXGun;
		}
	}
	static bool bLastCartoonGun = false;
	void Cartoongun() {
		bool bCartoonGun = CartoonGunbool;
		if (bCartoonGun || (!bCartoonGun && bCartoonGun != bLastCartoonGun)) {
			if (PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK)) {
				while (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("scr_rcbarry2")) {
					STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_rcbarry2");
					script::get_current()->yield();
				}
				Vector3 v0;
				Vector3 v1;
				auto weaponIndexed = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(PLAYER::PLAYER_PED_ID(), 0);
				MISC::GET_MODEL_DIMENSIONS(WEAPON::GET_SELECTED_PED_WEAPON(PLAYER::PLAYER_PED_ID()), &v0, &v1);
				GRAPHICS::USE_PARTICLE_FX_ASSET("scr_rcbarry2");
				GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_PED_BONE("muz_clown", weaponIndexed, Vector3((v0.x - v1.x) / 2.0f + 0.7f, 0.f, 0.f), Vector3(0.f, 180.f, 0.f), 1, 1, 1, 1, 1);
			}
			if (!bCartoonGun)
				STREAMING::REMOVE_NAMED_PTFX_ASSET("scr_rcbarry2");
			bLastCartoonGun = bCartoonGun;
		}
	}
	void Gravitygun() {
		if (Gravitygunbool) {
			static Entity ent = 0;
			static Vector3 location;
			static Vector3 other;
			static float dist;
			static const int scroll = 0;
			static const int controlsgravitygun[] = { 14, 15, 24 };
			double multiplier = 3.0;
			// ZOOMED IN
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM)) {
				PLAYER::DISABLE_PLAYER_FIRING(PLAYER::GET_PLAYER_INDEX(), TRUE);
				for (int control : controlsgravitygun)
					PAD::DISABLE_CONTROL_ACTION(0, control, true);
				location = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE);
				// Attack RELEASED
				if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK) && ent == 0) {
					if (raycast(&ent)) {
						if (ENTITY::IS_ENTITY_A_PED(ent) && PED::IS_PED_A_PLAYER(ent)) {
							ent = 0;
							//g_dxUiManager.sendNotify("Weapon", "You can't move player entities!");
						}
						else {
							other = ENTITY::GET_ENTITY_COORDS(ent, TRUE);
							dist = math::distance_between_vectors(location, other);
							if (dist > 500.f) {
								ent = 0;
								//g_dxUiManager.sendNotify("Weapon", "Entity is too far.");
							}
							else {
								if (take_control_of(ent) && ENTITY::IS_ENTITY_A_PED(ent) && !PED::IS_PED_RAGDOLL(ent)) {
									TASK::SET_HIGH_FALL_TASK(ent, 0, 0, 0);
									//g_dxUiManager.sendNotify("Weapon", "Selected entity at crosshair.");
								}
							}
						}
					}
					else {
						ent = 0;
						//g_dxUiManager.sendNotify("Weapon", "No entity found.");
					}
				}
				if (ENTITY::DOES_ENTITY_EXIST(ent)) {
					if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_WEAPON_WHEEL_NEXT))
						dist -= 5;
					if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_WEAPON_WHEEL_PREV))
						dist += 5;
					if (!take_control_of(ent)) {
						ent = 0;
						return;//g_dxUiManager.sendNotify("Weapon", "Failed to take control of entity.");
					}
					ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(ent, FALSE, FALSE);
					other = ENTITY::GET_ENTITY_COORDS(ent, true);
					Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
					float pitch = math::deg_to_rad(rot.x); // vertical
					float yaw = math::deg_to_rad(rot.z + 90); // horizontal
					Vector3 velocity;
					velocity.x = location.x + (dist * cos(pitch) * cos(yaw)) - other.x;
					velocity.y = location.y + (dist * sin(yaw) * cos(pitch)) - other.y;
					velocity.z = location.z + (dist * sin(pitch)) - other.z;
					ENTITY::SET_ENTITY_VELOCITY(ent, Vector3(velocity.x * (float)multiplier, velocity.y * (float)multiplier, velocity.z * (float)multiplier));
					ENTITY::SET_ENTITY_ALPHA(ent, 160, 0);
				}
			}
			else if (ent != 0 && take_control_of(ent)) {
				ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(ent, TRUE, TRUE);
				ENTITY::RESET_ENTITY_ALPHA(ent);
				ent = 0;
				//g_dxUiManager.sendNotify("Weapon", "Released entity.");
			}
		}
	}
	void RGBWep() {
		if (RainbowWeaponsBool) {
			auto playerPed = PLAYER::PLAYER_PED_ID();
			for (const auto& allweapons : weaponHashes) {
				if (WEAPON::HAS_PED_GOT_WEAPON(playerPed, allweapons, FALSE)) {
					WEAPON::SET_PED_WEAPON_TINT_INDEX(playerPed, allweapons, math::rng(0, 7).get<int>());
				}
			}
		}
	}
	void Airstrikegunwep() {
		if (Airstrikebool) {
			Player MyPlayer = PLAYER::PLAYER_PED_ID();
			Vector3 ShootCoord;
			while (!WEAPON::HAS_WEAPON_ASSET_LOADED(0xF8A3939F)) {
				WEAPON::REQUEST_WEAPON_ASSET(0xF8A3939F, 31, 0);
				script::get_current()->yield();
			}
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &ShootCoord)) {
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(Vector3(ShootCoord.x, ShootCoord.y, ShootCoord.z + 15), Vector3(ShootCoord.x, ShootCoord.y, ShootCoord.z), GetWeaponDamage(0xF8A3939F), 1, 0xF8A3939F, MyPlayer, 1, 1, 100);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(Vector3(ShootCoord.x, ShootCoord.y + 5, ShootCoord.z + 15), Vector3(ShootCoord.x, ShootCoord.y, ShootCoord.z), GetWeaponDamage(0xF8A3939F), 1, 0xF8A3939F, MyPlayer, 1, 1, 130);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(Vector3(ShootCoord.x + 3, ShootCoord.y, ShootCoord.z + 15), Vector3(ShootCoord.x, ShootCoord.y, ShootCoord.z), GetWeaponDamage(0xF8A3939F), 1, 0xF8A3939F, MyPlayer, 1, 1, 80);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(Vector3(ShootCoord.x - 5, ShootCoord.y, ShootCoord.z + 15), Vector3(ShootCoord.x, ShootCoord.y, ShootCoord.z), GetWeaponDamage(0xF8A3939F), 1, 0xF8A3939F, MyPlayer, 1, 1, 110);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(Vector3(ShootCoord.x, ShootCoord.y - 2, ShootCoord.z + 15), Vector3(ShootCoord.x, ShootCoord.y, ShootCoord.z), GetWeaponDamage(0xF8A3939F), 1, 0xF8A3939F, MyPlayer, 1, 1, 120);
			}
		}
	}
	void Triggerbot() {
		if (!TRIGGERBOT)
			return;

		Entity AimedAtEntity;
		if (!PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(PLAYER::PLAYER_ID(), &AimedAtEntity))
			return;

		if (!ENTITY::IS_ENTITY_A_PED(AimedAtEntity) || PED::IS_PED_DEAD_OR_DYING(AimedAtEntity, false))
			return;

		Vector3 aimPosition;
		switch (TriggerBotType) {
		case 0:
			if (!PED::IS_PED_A_PLAYER(AimedAtEntity))
				aimPosition = PED::GET_PED_BONE_COORDS(AimedAtEntity, (int)PedBones::SKEL_Head, Vector3(0.1f, 0.0f, 0.0f));
			break;
		case 1:
			if (PED::IS_PED_A_PLAYER(AimedAtEntity))
				aimPosition = PED::GET_PED_BONE_COORDS(AimedAtEntity, (int)PedBones::SKEL_Head, Vector3(0.1f, 0.0f, 0.0f));
			break;
		case 2:
			aimPosition = PED::GET_PED_BONE_COORDS(AimedAtEntity, (int)PedBones::SKEL_Head, Vector3(0.1f, 0.0f, 0.0f));
			break;
		}

		if (aimPosition != Vector3()) {
			PED::SET_PED_SHOOTS_AT_COORD(PLAYER::PLAYER_PED_ID(), aimPosition, true);
		}
	}
	static const int controlsfordeletegun[] = { 14, 15, 24 };
	void Deletegun() {
		if (Deletegunbool && PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM)) {
			PLAYER::DISABLE_PLAYER_FIRING(PLAYER::GET_PLAYER_INDEX(), TRUE);
			for (int control : controlsfordeletegun)
				PAD::DISABLE_CONTROL_ACTION(0, control, true);
			if (PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, (int)ControllerInputs::INPUT_ATTACK)) {
				Entity entity;
				if (raycast(&entity)) {
					if (ENTITY::IS_ENTITY_A_PED(entity) && PED::IS_PED_A_PLAYER(entity)) {
						//	g_dxUiManager.sendNotify("Weapon", "You can't delete player entities!");
					}
					else {
						auto player = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE);
						auto entLoc = ENTITY::GET_ENTITY_COORDS(entity, TRUE);
						auto dist = math::distance_between_vectors(player, entLoc);
						if (dist <= 500.f && take_control_of(entity)) {
							delete_entity(entity);
						}
						else {
							//	g_dxUiManager.sendNotify("Weapon", "Entity is too far.");
						}
					}
				}
				//else g_dxUiManager.sendNotify("Weapon", "No Entity found.");
			}
		}
	}
	inline void cage_opt(Ped ped) {
		rage::joaat_t hash = "prop_gold_cont_01"_joaat;
		auto location = ENTITY::GET_ENTITY_COORDS(ped, TRUE);
		OBJECT::CREATE_OBJECT_NO_OFFSET(hash, location, TRUE, FALSE, FALSE);
	}
	void Cagegun() {
		if (Cagegunbool && PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM)) {
			if (PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, (int)ControllerInputs::INPUT_ATTACK)) {
				Entity entity;
				if (raycast(&entity)) {
					if (ENTITY::IS_ENTITY_A_PED(entity))
						cage_opt(entity);
				}
				//else g_dxUiManager.sendNotify("Weapon", "No Entity found.");
			}
		}
	}
	//Vehicle
	static bool bLastVehGodmode = false;
	void vehGodmode() {
		bool bVehGodmode = VehicleGodmode;
		if (bVehGodmode || (!bVehGodmode && bVehGodmode != bLastVehGodmode)) {
			gta_util::execute_as_script("main_persistent"_joaat, [] {
				auto ped = PLAYER::PLAYER_PED_ID();
				auto veh = PED::GET_VEHICLE_PED_IS_USING(ped);
				ENTITY::SET_ENTITY_INVINCIBLE(veh, VehicleGodmode);
				});
			bLastVehGodmode = VehicleGodmode;
		}
	}
	static constexpr float hornBoostSpeedDefault = 10.f;
	static float hornBoostSpeed = hornBoostSpeedDefault;
	static constexpr float hostBoostSpeedMax = 200.f;
	void vehicle_horn_boost() {
		if (hornboost) {
			const Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
			if (veh == NULL) {
				hornBoostSpeed = hornBoostSpeedDefault;
				return;
			}
			if (PAD::IS_CONTROL_JUST_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HORN))
				hornBoostSpeed = ENTITY::GET_ENTITY_SPEED(veh);
			if (PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HORN)) {
				if (hornBoostSpeed < hostBoostSpeedMax) hornBoostSpeed++;
				const auto velocity = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, Vector3(0.f, hornBoostSpeed, 0.f)) - ENTITY::GET_ENTITY_COORDS(veh, true);
				ENTITY::SET_ENTITY_VELOCITY(veh, velocity);
			}
			else if (PAD::IS_CONTROL_JUST_RELEASED(0, (int)ControllerInputs::INPUT_VEH_HORN))
				hornBoostSpeed = hornBoostSpeedDefault;
		}
	}
	bool repair(Vehicle veh) {
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh) || !take_control_of(veh, 0)) {
			return false;
		}
		VEHICLE::SET_VEHICLE_FIXED(veh);
		VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(veh);
		VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
		return true;
	}
	void fixLoop() {
		if (vehiclefixLoopBool && VEHICLE::GET_DOES_VEHICLE_HAVE_DAMAGE_DECALS(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID())))
			repair(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()));
	}
	void rainbowvehicle() {
		if (RainbowVehicleBool) {
			Ped playerPed = PLAYER::PLAYER_PED_ID();
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(playerPed, FALSE);
			switch (RainbowSelect) {
			case 0: {
				if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
					RGBFadeInt();
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, redcolorInt, greencolorInt, bluecolorInt);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, redcolorInt, greencolorInt, bluecolorInt);
					VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, redcolorInt, greencolorInt, bluecolorInt);
					VEHICLE::GET_VEHICLE_NEON_ENABLED(veh, 1);
					for (int index{}; index != 7; index++) {
						VEHICLE::SET_VEHICLE_NEON_ENABLED(veh, index, 1);
					}
					VEHICLE::SET_VEHICLE_NEON_COLOUR(veh, redcolorInt, greencolorInt, bluecolorInt);
				}
			} break;
			case 1: {
				if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, math::rng(0, 255).get<int>(), math::rng(0, 255).get<int>(), math::rng(0, 255).get<int>());
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, math::rng(0, 255).get<int>(), math::rng(0, 255).get<int>(), math::rng(0, 255).get<int>());
					VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, math::rng(0, 255).get<int>(), math::rng(0, 255).get<int>(), math::rng(0, 255).get<int>());
					for (int index{}; index != 7; index++) {
						VEHICLE::SET_VEHICLE_NEON_ENABLED(veh, index, 1);
					}
					VEHICLE::SET_VEHICLE_NEON_COLOUR(veh, math::rng(0, 255).get<int>(), math::rng(0, 255).get<int>(), math::rng(0, 255).get<int>());
				}
			} break;
			}
		}
	}
	static bool bLastScrutch = false;
	void Scrutch() {
		bool bScrutch = Scrutchbool;
		if (bScrutch || (!bScrutch && bScrutch != bLastScrutch)) {
			if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0)) {
				ENTITY::SET_ENTITY_RENDER_SCORCHED(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), Scrutchbool);
			}
			bLastScrutch = Scrutchbool;
		}
	}
	void Stickymicky() {
		if (StickyBool && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), FALSE)) {
			auto myveh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
			auto calculateHieght = ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(myveh);
			if (calculateHieght <= 1.5f)
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(myveh, true);
		}
	}
	static bool bLastSeatbelt = false;
	void SeatBelt() {
		bool bSeatBelt = Seatbeltbool;
		if (bSeatBelt || (!bSeatBelt && bSeatBelt != bLastSeatbelt)) {
			PED::SET_PED_CONFIG_FLAG(PLAYER::PLAYER_PED_ID(), 32, bSeatBelt);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::PLAYER_PED_ID(), bSeatBelt);
			bLastSeatbelt = Seatbeltbool;
		}
	}
	static bool bLastDrift = false;
	void Driftmode() {
		bool bDrift = DriftBool;
		if (bDrift || (!bDrift && bDrift != bLastDrift)) {
			VEHICLE::SET_VEHICLE_REDUCE_GRIP(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE), DriftBool);
			bLastDrift = DriftBool;
		}
	}
	const int NUM_PLATE_SYMBOLS = 35;
	std::string GenerateRandomPlateText() {
		std::string plateText;
		for (int i = 0; i < 9; i++) {
			plateText += plateSymbols[rand() % NUM_PLATE_SYMBOLS];
		}
		return plateText;
	}
	void MatrixPlate() {
		if (MatrixPlates && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0)) {
			std::string plateText = GenerateRandomPlateText();
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), plateText.c_str());
		}
	}
	void Handbreak() {
		if (HandBreakBool && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0) && GetKeyState(VK_MENU)) {
			auto handbreakk = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
			VEHICLE::SET_VEHICLE_FORWARD_SPEED(handbreakk, 0.f);
		}
	}
	//
	/*static bool bLastCamera = false;
	static Cam Camera;
	void CustomCamera() {
		if (DisplayPlayersCam == bLastCamera)
			return;
		if (!CAM::DOES_CAM_EXIST(Camera)) {
			gta_util::execute_as_script(*g_pointers->m_isSessionActive ? "freemode"_joaat : "main_persistent"_joaat, [] {
				Camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", TRUE);
				});
		}
		auto Player = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_selectedPlayer);
		Vector3 cameraPos = ENTITY::GET_ENTITY_COORDS(Player, true) + ENTITY::GET_ENTITY_FORWARD_VECTOR(Player) * 2.5f;
		CAM::SET_CAM_COORD(Camera, cameraPos);
		CAM::POINT_CAM_AT_ENTITY(Camera, Player, 0.f, 0.f, 0.f, TRUE);
		CAM::SET_CAM_ACTIVE(Camera, DisplayPlayersCam);
		CAM::RENDER_SCRIPT_CAMS(DisplayPlayersCam, true, 718, true, false, false);
		STREAMING::SET_FOCUS_ENTITY(DisplayPlayersCam ? Player : PLAYER::PLAYER_PED_ID());
		if (!DisplayPlayersCam) {
			CAM::DESTROY_CAM(Camera, TRUE);
		}
		bLastCamera = DisplayPlayersCam;
	}*/
	void system_desync_kick_protection() {
		memset(&gta_util::get_network()->m_game_complaint_mgr.m_host_tokens_complained, 0, 64 * sizeof(std::uint64_t));
		if (!g_player_service->m_player_to_use_complaint_kick
			|| !g_player_service->m_player_to_use_complaint_kick->get()->get_net_data())
			gta_util::get_network()->m_game_complaint_mgr.m_num_tokens_complained = 0;
		else {
			gta_util::get_network()->m_game_complaint_mgr.m_num_tokens_complained = 1;
			gta_util::get_network()->m_game_complaint_mgr.m_host_tokens_complained[0] =
				g_player_service->m_player_to_use_complaint_kick->get()->get_net_data()->m_host_token;
		}
		auto old = gta_util::get_network()->m_game_complaint_mgr.m_host_token;
		if (gta_util::get_network()->m_game_session_state > 3 && gta_util::get_network()->m_game_session_state < 6
			&& g_player_service->m_player_to_use_complaint_kick && g_player_service->get_self()->is_valid()
			&& !g_player_service->get_self()->is_host()) {
			g_pointers->m_reset_network_complaints(&gta_util::get_network()->m_game_complaint_mgr);
		}
	}
	void V_DisableAllActions() {
		if (DisableAllActions)
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
	}
	//void PedHeadshots() {
	//	if (drawheadshot) {
	//		auto headshotTXD = "CHAR_DEFAULT";
	//		auto pedheadshotGlobalHandle = *globals(1666485).at(g_selectedPlayer, 5).at(2).as<int*>();
	//		for (int i = 0; i <= 150; i += 5) {
	//			if (pedheadshotGlobalHandle == g_selectedPlayer) {
	//				int logo = *globals(1666485).at(i).at(1).as<int*>();
	//				if (PED::IS_PEDHEADSHOT_VALID(logo) && PED::GET_PEDHEADSHOT_TXD_STRING(logo) != nullptr) {
	//					headshotTXD = PED::GET_PEDHEADSHOT_TXD_STRING(logo);
	//				}
	//			}
	//		}
	//		GRAPHICS::DRAW_SPRITE(headshotTXD, headshotTXD, Vector2(0.5f, 0.5f), 0.1f, 0.1f, 0.f, 255, 255, 255, 255, FALSE, FALSE);
	//	}
	//}
	void movewithmouse() {
		if (move_withmousebool) {
			g_dxUiManager.m_pos.x = ImGui::GetMousePos().x;
			g_dxUiManager.m_pos.y = ImGui::GetMousePos().y;
		}
	}
	void tick() {
		while (true) {
			//Self
			godMode();
			invisibility();
			neverWanted();
			noRagdoll();
			swimInAir();
			superRun();
			fastSwim();
			fastRun();
			offRadar();
			NoClip();
			self_free_cam();
			healthRegen();
			unlimitedOxygen();
			selfClean();
			mobileRadio();
			noCollision();
			noWaterCollision();
			selfFastRespawn();
			infiniteAmmo();
			rapidFire();
			SeatBelt();
			Triggerbot();
			Gravitygun();
			Cagegun();
			Deletegun();
			Teleportgun();
			Cartoongun();
			PTFXgun();
			Stealgun();
			MCgun();
			Airstrikegunwep();
			RGBWep();
			RPGun();
			vehGodmode();
			vehicle_horn_boost();
			fixLoop();
			rainbowvehicle();
			Scrutch();
			Stickymicky();
			Driftmode();
			MatrixPlate();
			Handbreak();
			spectatePlayer();
			forceSessionHost();
			hudTransitionState();
			seamlessJoin();
			V_DisableAllActions();
			system_desync_kick_protection();
			movewithmouse();
			script::get_current()->yield();
		}
	}
}