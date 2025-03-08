#pragma once
#include "GTAV-Classes-master/rage/joaat.hpp"
#include "GtaUtill.hpp"
#include "Math.hpp"
#include "Natives.hpp"
#include "Fibers/script.hpp"
namespace base
{
	inline void cage_ped(Ped ped) {
		Hash hash = "prop_gold_cont_01"_joaat;
		Vector3 location = ENTITY::GET_ENTITY_COORDS(ped, true);
		OBJECT::CREATE_OBJECT(hash, Vector3(location.x, location.y, location.z - 1.f), true, false, false);
	}
	inline void clean_ped(Ped ped) {
		Ped player_ped = PLAYER::PLAYER_PED_ID();
		PED::CLEAR_PED_BLOOD_DAMAGE(player_ped);
		PED::CLEAR_PED_WETNESS(player_ped);
		PED::CLEAR_PED_ENV_DIRT(player_ped);
		PED::RESET_PED_VISIBLE_DAMAGE(player_ped);
	}
	inline void delete_entity(Entity ent) {
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ent, 1, 1);
		ENTITY::DETACH_ENTITY(ent, 1, 1);
		ENTITY::SET_ENTITY_VISIBLE(ent, false, false);
		NETWORK::NETWORK_SET_ENTITY_ONLY_EXISTS_FOR_PARTICIPANTS(ent, true);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, Vector3(0.f, 0.f, 0.f), FALSE, FALSE, FALSE);
		if (ENTITY::IS_ENTITY_A_PED(ent)) {
			PED::DELETE_PED(&ent);
		}
		else if (ENTITY::IS_ENTITY_A_VEHICLE(ent)) {
			VEHICLE::DELETE_VEHICLE(&ent);
		}
		else if (ENTITY::IS_ENTITY_AN_OBJECT(ent)) {
			OBJECT::DELETE_OBJECT(&ent);
		}
		else {
			ENTITY::DELETE_ENTITY(&ent);
		}
	}
	inline bool raycast(Entity* ent) {
		BOOL hit;
		Vector3 endCoords;
		Vector3 surfaceNormal;
		Vector3 camCoords = CAM::GET_GAMEPLAY_CAM_COORD();
		Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
		Vector3 dir = math::rotation_to_direction(rot);
		Vector3 farCoords;
		farCoords.x = camCoords.x + dir.x * 1000;
		farCoords.y = camCoords.y + dir.y * 1000;
		farCoords.z = camCoords.z + dir.z * 1000;
		int ray = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(camCoords,
			farCoords,
			-1,
			0,
			7);
		SHAPETEST::GET_SHAPE_TEST_RESULT(ray, &hit, &endCoords, &surfaceNormal, ent);

		return (bool)hit;
	}

	inline bool network_has_control_of_entity(rage::netObject* net_object)
	{
		return !net_object || !net_object->m_next_owner_id && (net_object->m_control_id == -1);
	}

	inline bool take_control_of(Entity ent, int timeout = 300) {
		auto hnd = g_pointers->m_handleToPointer(ent);
		if (!hnd || !hnd->m_net_object || !*g_pointers->m_isSessionActive)
			return false;

		if (network_has_control_of_entity(hnd->m_net_object))
			return true;

		int i = 0;

		while (!network_has_control_of_entity(hnd->m_net_object) && (timeout == 0 || i < timeout)) {
			g_pointers->m_requestControl(hnd->m_net_object);
			script::get_current()->yield();
			++i;
		}

		return network_has_control_of_entity(hnd->m_net_object);
	}

	//inline std::vector<Entity> get_entities(bool vehicles, bool peds, bool props = false)
	//{
	//	std::vector<Entity> target_entities;
	//	target_entities.clear();
	//	if (vehicles)
	//	{
	//		for (auto vehicle : *g_pointers->m_replayInterface->m_vehicle_interface->m_max_vehicles)
	//		{
	//			if (!vehicle || vehicle == gta_util::get_local_vehicle())
	//				continue;

	//			target_entities.push_back(g_pointers->m_pointerToHandle(vehicle));
	//		}
	//	}
	//	if (peds)
	//	{
	//		for (auto ped : pools::get_all_peds())
	//		{
	//			// make sure to not include ourselves
	//			if (!ped || ped == gta_util::get_local_ped())
	//				continue;

	//			target_entities.push_back(g_pointers->m_pointerToHandle(ped));
	//		}
	//	}

	//	if (props)
	//	{
	//		for (auto prop : pools::get_all_props())
	//		{
	//			if (!prop)
	//				continue;

	//			target_entities.push_back(g_pointers->m_pointerToHandle(prop));
	//		}
	//	}
	//	return target_entities;
	//}
	inline bool load_ground_at_3dcoord(Vector3& location) {
		float groundZ;
		const uint8_t attempts = 10;
		for (uint8_t i = 0; i < attempts; i++) {
			// Only request a collision after the first try failed because the location might already be loaded on first attempt.
			for (uint16_t z = 0; i && z < 1000; z += 100) {
				STREAMING::REQUEST_COLLISION_AT_COORD(Vector3(location.x, location.y, (float)z));
				script::get_current()->yield();
			}
			if (MISC::GET_GROUND_Z_FOR_3D_COORD(Vector3(location.x, location.y, 1000.f), &groundZ, false, false)) {
				location.z = groundZ + 1.f;
				return true;
			}
			script::get_current()->yield();
		}
		location.z = 1000.f;
		return false;
	}
	inline double distance_to_middle_of_screen(const rage::fvector2& screen_pos)
	{
		double cumulative_distance{};

		if (screen_pos.x > 0.5)
			cumulative_distance += screen_pos.x - 0.5;
		else
			cumulative_distance += 0.5 - screen_pos.x;

		if (screen_pos.y > 0.5)
			cumulative_distance += screen_pos.y - 0.5;
		else
			cumulative_distance += 0.5 - screen_pos.y;

		return cumulative_distance;
	}

	/*inline Entity get_entity_closest_to_middle_of_screen() {
		Entity closest_entity{};
		double distance = 1;
		auto replayInterface = *g_pointers->m_replayInterface;
		auto vehicleInterface = replayInterface->m_vehicle_interface;
		auto pedInterface = replayInterface->m_ped_interface;
		for (const auto veh : (*vehicleInterface->m_vehicle_list)) {
			if (veh.m_entity_ptr) {
				Vehicle handle = g_pointers->m_pointerToHandle(veh.m_entity_ptr);
				Vector3 pos = ENTITY::GET_ENTITY_COORDS(handle, 1);
				rage::fvector2 screenpos;
				HUD::GET_HUD_SCREEN_POSITION_FROM_WORLD_POSITION(pos, &screenpos.x, &screenpos.y);
				if (distance_to_middle_of_screen(screenpos) < distance && ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(PLAYER::PLAYER_PED_ID(), handle, 17)) {
					closest_entity = handle;
					distance = distance_to_middle_of_screen(screenpos);
				}
			}
		}
		for (auto ped : *pedInterface->m_ped_list) {
			if (ped.m_entity_ptr) {
				Vehicle handle = g_pointers->m_pointerToHandle(ped.m_entity_ptr);
				Vector3 pos = ENTITY::GET_ENTITY_COORDS(handle, 1);
				rage::fvector2 screenpos;
				HUD::GET_HUD_SCREEN_POSITION_FROM_WORLD_POSITION(pos, &screenpos.x, &screenpos.y);
				if (distance_to_middle_of_screen(screenpos) < distance && ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(PLAYER::PLAYER_PED_ID(), handle, 17) && handle != PLAYER::PLAYER_PED_ID()) {
					closest_entity = handle;
					distance = distance_to_middle_of_screen(screenpos);
				}
			}
		}
		return closest_entity;
	}*/
}