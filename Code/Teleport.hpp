#pragma once
#include "Entity.hpp"
#include "Enums.hpp"
#include "Natives.hpp"
#include "ui/dxUiManager.hpp"
#include "Translations.hpp"
#include "Services/Player/player.hpp"
#include "MainScript.hpp"
#include "Features.hpp"
namespace blip
{
	inline bool get_blip_location(Vector3& location, int sprite, int color = -1) {
		Blip blip;
		for (blip = HUD::GET_FIRST_BLIP_INFO_ID(sprite); HUD::DOES_BLIP_EXIST(blip) && color != -1 && HUD::GET_BLIP_COLOUR(blip) != color; blip = HUD::GET_NEXT_BLIP_INFO_ID(sprite));
		if (!HUD::DOES_BLIP_EXIST(blip) || (color != -1 && HUD::GET_BLIP_COLOUR(blip) != color))
			return false;
		location = HUD::GET_BLIP_COORDS(blip);
		return true;
	}
	inline bool get_objective_location(Vector3& location) {
		if (get_blip_location(location, (int)BlipIcons::Circle, (int)BlipColors::YellowMission))
			return true;
		if (get_blip_location(location, (int)BlipIcons::Circle, (int)BlipColors::YellowMission2))
			return true;
		if (get_blip_location(location, (int)BlipIcons::Circle, (int)BlipColors::Mission))
			return true;
		if (get_blip_location(location, (int)BlipIcons::RaceFinish, (int)BlipColors::None))
			return true;
		if (get_blip_location(location, (int)BlipIcons::Circle, (int)BlipColors::Green))
			return true;
		if (get_blip_location(location, (int)BlipIcons::Circle, (int)BlipColors::Blue))
			return true;
		if (get_blip_location(location, (int)BlipIcons::CrateDrop))
			return true;
		for (static const int blips[] = { 1, 57, 128, 129, 130, 143, 144, 145, 146, 271, 286, 287, 288 }; const auto & blip : blips) {
			if (get_blip_location(location, blip, 5))
				return true;
		}
		return false;
	}
}
namespace base
{
	inline int TeleportState = 0;
	inline const char* TeleportAnimationNames[] = {
		"Normal",
		"Fade",
		"Swoop Down",
		"Smooth",
	};
	inline void set_mp_bitset(Vehicle veh)
	{
		DECORATOR::DECOR_SET_INT(veh, "MPBitset", 0);
		DECORATOR::DECOR_SET_INT(veh, "RandomId", gta_util::get_local_ped()->m_net_object->m_object_id);
		ENTITY::SET_ENTITY_SHOULD_FREEZE_WAITING_ON_COLLISION(veh, true);
		auto networkId = NETWORK::VEH_TO_NET(veh);
		if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(veh))
			NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, true);
		VEHICLE::SET_VEHICLE_IS_STOLEN(veh, FALSE);
	}
	inline Vehicle spawn(Hash hash, Vector3 location, float heading, bool is_networked = true, bool script_veh = false)
	{
		for (uint8_t i = 0; !STREAMING::HAS_MODEL_LOADED(hash) && i < 25; i++)
		{
			STREAMING::REQUEST_MODEL(hash);
			script::get_current()->yield();
		}

		if (!STREAMING::HAS_MODEL_LOADED(hash))
		{
			return 0;
		}

		static Vehicle veh;
		gta_util::execute_as_script(*g_pointers->m_isSessionActive ? "freemode"_joaat : "main_persistent"_joaat, [hash, location, heading, is_networked, script_veh] {
			veh = VEHICLE::CREATE_VEHICLE(hash, location, heading, is_networked, script_veh, false);
			});

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

		if (*g_pointers->m_isSessionActive)
		{
			set_mp_bitset(veh);
		}

		return veh;
	}
	inline bool teleport_player_to_coords(player_ptr player, Vector3 coords)
	{
		Entity ent;
		if (*g_pointers->m_isSessionActive)
			ent = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id());
		else
			ent = PLAYER::PLAYER_PED_ID();

		if (ent == PLAYER::PLAYER_PED_ID() || ent == PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()))
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, Vector3(coords.x, coords.y, coords.z + 1.f), 0, 0, 0);

		if (ENTITY::IS_ENTITY_DEAD(ent, true))
		{
			MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), TRANSLATE("TELEPORT_PLAYER_IS_DEAD"));
			return false;
		}

		if (PED::IS_PED_IN_ANY_VEHICLE(ent, true))
		{
			ent = PED::GET_VEHICLE_PED_IS_IN(ent, false);

			if (take_control_of(ent)) {
				load_ground_at_3dcoord(coords);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, Vector3(coords.x, coords.y, coords.z + 1.f), 0, 0, 0);
			}

			else
				MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), TRANSLATE("FAILED_TO_TAKE_CONTROL_OF_VEHICLE"));

			return true;
		}
		else
		{
			auto hnd = spawn("RCBANDITO"_joaat, *player->get_ped()->get_position(), 0.0f, true);
			ENTITY::SET_ENTITY_VISIBLE(hnd, false, false);
			ENTITY::SET_ENTITY_COLLISION(hnd, false, false);
			ENTITY::FREEZE_ENTITY_POSITION(hnd, true);

			auto obj_id = player->get_ped()->m_net_object->m_object_id;
			auto veh_id = g_pointers->m_handleToPointer(hnd)->m_net_object->m_object_id;
			remote_player_teleport remote_tp = { obj_id, {coords.x, coords.y, coords.z} };
			g_mainScript.m_remote_player_teleports.emplace(veh_id, remote_tp);

			if ((player->is_valid() && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id()), false))
				|| PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(player->id()))
				g_pointers->m_clearPedTasksNetwork(player->get_ped(), true);

			g_pointers->m_clearPedTasksNetwork(player->get_ped(), true);

			for (int i = 0; i < 15; i++)
			{
				script::get_current()->yield(50ms);

				if (auto ptr = g_pointers->m_handleToPointer(hnd))
				{
					if (auto netobj = ptr->m_net_object)
					{
						g_pointers->m_migrateObject(player->get_net_game_player(), netobj, 3);
					}
				}
			}

			if (take_control_of(hnd))
				delete_entity(hnd);

			std::erase_if(g_mainScript.m_remote_player_teleports, [veh_id](auto& obj) {
				return obj.first == veh_id;
				});

			return true;
		}
	}
	inline bool bring_player(player_ptr player)
	{
		return teleport_player_to_coords(player, *g_player_service->get_self()->get_ped()->get_position());
	}
	inline bool into_vehicle(Vehicle veh) {
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh)) {
			MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), TRANSLATE("INVALID_VEHICLE"));
			return false;
		}
		int seat_index = -1;
		if (!VEHICLE::IS_VEHICLE_SEAT_FREE(veh, -1, true)) {
			if (!VEHICLE::IS_VEHICLE_SEAT_FREE(veh, -2, true)) {
				MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), TRANSLATE("NO_SEATS_AVAILABLE"));
				return false;
			}
			seat_index = -2;
		}
		Vector3 location = ENTITY::GET_ENTITY_COORDS(veh, TRUE);
		Ped ped = PLAYER::PLAYER_PED_ID();
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ped, location, FALSE, FALSE, FALSE);
		script::get_current()->yield();
		PED::SET_PED_INTO_VEHICLE(ped, veh, seat_index);
		return true;
	}
	inline void to_coords(Vector3 location) {
		auto selfPed = PLAYER::PLAYER_PED_ID();
		auto vehicle = PED::GET_VEHICLE_PED_IS_USING(selfPed);
		auto inVehicle = PED::IS_PED_IN_ANY_VEHICLE(selfPed, 0);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : selfPed, Vector3(location.x, location.y, location.z + 1.f), FALSE, FALSE, FALSE);
	}
	inline bool to_blip(int sprite, int color = -1) {
		Vector3 location;
		auto selfPed = PLAYER::PLAYER_PED_ID();
		auto vehicle = PED::GET_VEHICLE_PED_IS_USING(selfPed);
		auto inVehicle = PED::IS_PED_IN_ANY_VEHICLE(selfPed, 0);
		if (!blip::get_blip_location(location, sprite, color))
			return false;
		if (sprite == (int)BlipIcons::Waypoint)
			load_ground_at_3dcoord(location);
		/*switch (TeleportState)
		{
		case 0: {
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : selfPed, location, FALSE, FALSE, FALSE);
		} break;
		case 1: {
			g_fiber_pool->queue_job([&inVehicle, &vehicle, &selfPed, &location] {
				CAM::DO_SCREEN_FADE_OUT(500);
				script::get_current()->yield(718ms);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : selfPed, location, FALSE, FALSE, FALSE);
				CAM::DO_SCREEN_FADE_IN(1000);
				});
		} break;
		case 2: {
			g_fiber_pool->queue_job([&] {
				static Ped clone;
				auto veh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
				clone = PED::CLONE_PED(PLAYER::PLAYER_PED_ID(), FALSE, FALSE, TRUE);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : clone, location, FALSE, FALSE, FALSE);
				NETWORK::NETWORK_FADE_OUT_ENTITY(PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0) ? veh : PLAYER::PLAYER_PED_ID(), false, true);
				STREAMING::START_PLAYER_SWITCH(PLAYER::PLAYER_PED_ID(), clone, ePlayerSwitchFlags::SWITCH_FLAG_SKIP_INTRO | ePlayerSwitchFlags::SWITCH_FLAG_SKIP_OUTRO, ePlayerSwitchTypes::SWITCH_TYPE_MEDIUM);
				while (STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS()) {
					script::get_current()->yield(1ms);
				}
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : selfPed, location, FALSE, FALSE, FALSE);
				delete_entity(clone);
				if (STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
					STREAMING::STOP_PLAYER_SWITCH();
				NETWORK::NETWORK_FADE_IN_ENTITY(PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0) ? veh : PLAYER::PLAYER_PED_ID(), 1, 0);
				STREAMING::SET_RESTORE_FOCUS_ENTITY(selfPed);
				});
		} break;
		case 3: {
			g_fiber_pool->queue_job([&] {
				Cam cameraHandle{};
				auto PosX = location.x;
				auto PosY = location.y;
				auto PosZ = location.z;
				if (!CAM::DOES_CAM_EXIST(cameraHandle)) {
					cameraHandle = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 0);
				}
				CAM::SET_CAM_COORD(cameraHandle, Vector3(PosX, PosY, PosZ + 1000.f));
				CAM::SET_CAM_ROT(cameraHandle, Vector3(-90.0f, 0.0f, 0.0f), 0.0f);
				CAM::SET_CAM_ACTIVE(cameraHandle, true);
				CAM::RENDER_SCRIPT_CAMS(true, 1, 1718, 1, 0, 0);
				CAM::POINT_CAM_AT_COORD(cameraHandle, Vector3(PosX, PosY, PosZ + 1000.f));
				ENTITY::SET_ENTITY_ROTATION(cameraHandle, -90.0f, 0.0f, 0.0f, 2, 1);
				CAM::STOP_CAM_POINTING(cameraHandle);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : selfPed, location, FALSE, FALSE, FALSE);
				script::get_current()->yield(1718ms);
				STREAMING::LOAD_SCENE(location);
				if (CAM::DOES_CAM_EXIST(cameraHandle)) {
					CAM::SET_CAM_ACTIVE(cameraHandle, false);
					CAM::RENDER_SCRIPT_CAMS(false, true, 1718, true, true, 0);
					CAM::DESTROY_CAM(cameraHandle, false);
					cameraHandle = NULL;
				}
				STREAMING::SET_RESTORE_FOCUS_ENTITY(PLAYER::PLAYER_PED_ID());
				});
		} break;*/
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : selfPed, location, FALSE, FALSE, FALSE);
		return true;
	}

	inline bool to_entity(player_ptr ent) {
		Vector3 location = *ent->get_ped()->get_position();
		auto selfPed = PLAYER::PLAYER_PED_ID();
		auto vehicle = PED::GET_VEHICLE_PED_IS_USING(selfPed);
		auto inVehicle = PED::IS_PED_IN_ANY_VEHICLE(selfPed, 0);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : selfPed, Vector3(location.x, location.y, location.z + 3.0f), FALSE, FALSE, FALSE);
		return true;
	}
	inline bool to_player(player_ptr player) {
		return to_entity(player);
	}
	inline bool to_waypoint() {
		if (!to_blip((int)BlipIcons::Waypoint)) {
			MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), TRANSLATE("NO_WAYPOINT_SET"));
			return false;
		}
		return true;
	}
	inline bool to_objective() {
		Vector3 location;
		auto selfPed = PLAYER::PLAYER_PED_ID();
		auto vehicle = PED::GET_VEHICLE_PED_IS_USING(selfPed);
		auto inVehicle = PED::IS_PED_IN_ANY_VEHICLE(selfPed, 0);
		if (!blip::get_objective_location(location)) {
			MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), TRANSLATE("NO_OBJECTIVE_SET"));
			return false;
		}
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(inVehicle ? vehicle : selfPed, location, FALSE, FALSE, FALSE);
		return false;
	}
	//Helpers
	inline void TeleportToPlayer(player_ptr player) {
		Vector3 coords = *player->get_ped()->get_position();
		features::SpectateBool = true;
		script::get_current()->yield(3000ms);
		if (load_ground_at_3dcoord(coords) && !g_mainScript.is_in_interior(g_player_service->get_selected())) {
			to_player(player);
		}
		features::SpectateBool = false;
	}
	inline void TeleportPlayerToMe(player_ptr player) {
		features::SpectateBool = true;
		script::get_current()->yield(3000ms);
		int cnt = 0;
		while (cnt < 10) {
			if (math::distance_between_vectors(*g_player_service->get_self()->get_ped()->get_position(), *player->get_ped()->get_position()) <= 5.f) {
				features::SpectateBool = false;
				return;
			}
			bring_player(player);
			script::get_current()->yield(350ms);
			cnt++;
		}
		features::SpectateBool = false;
	}
	inline void TeleportIntoPlayersVehicle(player_ptr player) {
		features::SpectateBool = true;
		script::get_current()->yield(3000ms);
		int cnt = 0;
		while (cnt != 10) {
			auto PLYR_IS_PED_IN_ANY_VEHICLE = PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id()), FALSE);
			auto PLYR_GET_VEHICLE_PED_IS_IN = PED::GET_VEHICLE_PED_IS_USING(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id()));
			auto ME_GET_VEHICLE_PED_IS_IN = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
			if (PLYR_IS_PED_IN_ANY_VEHICLE) {
				into_vehicle(PLYR_GET_VEHICLE_PED_IS_IN);
				if (ME_GET_VEHICLE_PED_IS_IN == PLYR_GET_VEHICLE_PED_IS_IN) {
					features::SpectateBool = false;
					break;
				}
			}
			else {
				features::SpectateBool = false;
				return;
			}
			script::get_current()->yield(350ms);
			cnt++;
		}
		features::SpectateBool = false;
	}
	inline void TeleportPlayerToWaypoint(player_ptr player) {
		features::SpectateBool = true;
		script::get_current()->yield(3000ms);
		int cnt = 0;
		Vector3 pos = *player->get_ped()->get_position();
		while (cnt != 10) {
			auto WaypointHandle = HUD::GET_FIRST_BLIP_INFO_ID(8);
			Vector3 WaypointPos = HUD::GET_BLIP_COORDS(WaypointHandle);
			if (HUD::DOES_BLIP_EXIST(WaypointHandle)) {
				load_ground_at_3dcoord(WaypointPos);
				teleport_player_to_coords(player, Vector3(WaypointPos.x, WaypointPos.y, WaypointPos.z + 3.f));
			}
			else
			{
				features::SpectateBool = false;
				return;
			}
			if (pos == WaypointPos) {
				features::SpectateBool = false;
				return;
			}
			cnt++;
		}
		features::SpectateBool = false;
	}
}