#pragma once
#include "Pointers.hpp"
#include "Natives.hpp"
#include "ScriptGlobal.hpp"
#include "Notify.hpp"
#include "Translations.hpp"
#include "GtaUtill.hpp"
#include "Fibers/script.hpp"
#include "Entity.hpp"
#include "Enums.hpp"
#include "GTAV-Classes-master/rage/joaat.hpp"
#include "Math.hpp"
static inline int m_remote_controller_vehicle = -1;
namespace base
{
	enum Sports : Hash {
		VEHICLE_BANSHEE = "BANSHEE"_joaat,
		VEHICLE_SENTINEL = "SENTINEL"_joaat
	};
	static const ankerl::unordered_dense::map<Hash, std::map<int, std::vector<int32_t>>> mod_blacklists = { {VEHICLE_BANSHEE, {{MOD_SPOILERS, {3, 4}}, {MOD_COLUMNSHIFTERLEVERS, {0, 1, 2, 3}}, {MOD_SPEAKERS, {0}}, {MOD_LIVERY, {15, 16}}}}, {VEHICLE_SENTINEL, {{MOD_SPOILERS, {4, 5}}, {MOD_COLUMNSHIFTERLEVERS, {0, 1, 2, 3}}, {MOD_SPEAKERS, {0}}, {MOD_LIVERY, {0, 1}}}} };
	inline bool check_mod_blacklist(Hash model, int mod_slot, int mod) {
		if (mod_blacklists.find(model) == mod_blacklists.end()) {
			return false;
		}
		auto veh_slot_blacklist = mod_blacklists.find(model)->second;
		if (!veh_slot_blacklist.contains(mod_slot)) {
			return false;
		}
		if (auto veh_mod_blacklist = veh_slot_blacklist.find(mod_slot)->second; std::find(veh_mod_blacklist.begin(), veh_mod_blacklist.end(), mod) != veh_mod_blacklist.end()) {
			return true;
		}
		return false;
	}
	inline void max_vehicle(Vehicle veh) { //thx chatgpt
		Hash model = ENTITY::GET_ENTITY_MODEL(veh);
		VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);
		VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TURBO, TRUE);
		VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TYRE_SMOKE, TRUE);
		VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_XENON_LIGHTS, TRUE);
		VEHICLE::SET_VEHICLE_WINDOW_TINT(veh, 1);
		VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, false);

		const int startSlot = MOD_SPOILERS;
		const int endSlot = MOD_LIGHTBAR;

		for (int slot = startSlot; slot <= endSlot; slot++) {
			if (slot == MOD_LIVERY) {
				continue;
			}

			int count = VEHICLE::GET_NUM_VEHICLE_MODS(veh, slot);
			if (count > 0) {
				int selected_mod = -1;
				for (int mod = count - 1; mod >= -1; mod--) {
					if (mod == -1 || !check_mod_blacklist(model, slot, mod)) {
						selected_mod = mod;
						break;
					}
				}

				if (selected_mod != -1) {
					VEHICLE::SET_VEHICLE_MOD(veh, slot, selected_mod, true);
				}
			}
		}
	}
	inline void max_vehicle_performance(Vehicle veh) {
		if (take_control_of(veh)) {
			VehicleModType perfomance_mods[] = { MOD_ENGINE, MOD_BRAKES, MOD_TRANSMISSION, MOD_SUSPENSION, MOD_ARMOR, MOD_NITROUS, MOD_TURBO };
			VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);
			for (auto mod_slot : perfomance_mods) {
				if (mod_slot != MOD_NITROUS && mod_slot != MOD_TURBO)
					VEHICLE::SET_VEHICLE_MOD(veh, mod_slot, VEHICLE::GET_NUM_VEHICLE_MODS(veh, mod_slot) - 1, true);
				else
					VEHICLE::TOGGLE_VEHICLE_MOD(veh, mod_slot, true);
			}
		}
	}
	inline bool Sitinvehicle = false;
	inline bool MaxVehiclespawn = false;
	inline bool bSpawnWithEffect = false;
	inline void SpawnVehicle(Hash model) {
		STREAMING::REQUEST_MODEL(model);
		for (uint8_t i = 25; i && !STREAMING::HAS_MODEL_LOADED(model); i--) {
			STREAMING::REQUEST_MODEL(model);
			script::get_current()->yield();
		}
		if (!STREAMING::HAS_MODEL_LOADED(model)) {
			MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("VEHICLE_SPAWN"), TRANSLATE("VEHICLE_MODEL_NOT_LOADED"));
			return;
		}
		if (!STREAMING::IS_MODEL_A_VEHICLE(model) || !STREAMING::IS_MODEL_VALID(model) || !STREAMING::IS_MODEL_IN_CDIMAGE(model)) {
			MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("VEHICLE_SPAWN"), TRANSLATE("IS_NOT_A_VEHICLE"), model);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
			return;
		}
		auto PedDirection = ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID());
		const auto location = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), Vector3(2.f, 2.f, 0.f));
		static Vehicle veh;
		gta_util::execute_as_script((*g_pointers->m_isSessionActive ? "freemode"_joaat : "main_persistent"_joaat), [=] {
			veh = VEHICLE::CREATE_VEHICLE(model, location, PedDirection + 90.f, true, false, false);
			});
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
		VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh, 0);
		VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh, shortBrandingName);
		ENTITY::SET_ENTITY_HEADING(veh, PedDirection);
		if (*g_pointers->m_isSessionActive) {
			DECORATOR::DECOR_SET_INT(veh, "MPBitset", 0);
			DECORATOR::DECOR_SET_INT(veh, "RandomId", gta_util::get_local_ped()->m_net_object->m_object_id);
			auto networkId = NETWORK::VEH_TO_NET(veh);
			if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(veh))
				NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, true);
			VEHICLE::SET_VEHICLE_IS_STOLEN(veh, FALSE);
		}
		if (Sitinvehicle) {
			PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), veh, -1);
		}
		VEHICLE::SET_VEHICLE_ENGINE_ON(veh, true, true, true);
		if (VEHICLE::IS_THIS_MODEL_A_HELI(model) || VEHICLE::IS_THIS_MODEL_A_PLANE(model))
			VEHICLE::SET_HELI_BLADES_FULL_SPEED(veh);
		if (VEHICLE::IS_THIS_MODEL_A_PLANE(model)) {
			VEHICLE::SET_VEHICLE_KEEP_ENGINE_ON_WHEN_ABANDONED(veh, true);
		}
		if (MaxVehiclespawn) {
			max_vehicle(veh);
		}
		if (bSpawnWithEffect && *g_pointers->m_isSessionActive) {
			NETWORK::NETWORK_FADE_IN_ENTITY(veh, TRUE, 1);
		}
	}
	struct disable_collisions {
		inline static memory::byte_patch* m_patch;
	};
	inline float mps_to_speed(float mps, SpeedUnit speed_unit) {
		switch (speed_unit) {
		case SpeedUnit::KMPH: return mps * 3.6f; break;
		case SpeedUnit::MIPH: return mps * 2.2369f; break;
		}
		return mps;
	}
	inline float speed_to_mps(float speed, SpeedUnit speed_unit) {
		switch (speed_unit) {
		case SpeedUnit::KMPH: return speed / 3.6f; break;
		case SpeedUnit::MIPH: return speed / 2.2369f; break;
		}
		return speed;
	}
	inline Vector3 get_spawn_location(bool spawn_inside, Hash hash, Ped ped = PLAYER::PLAYER_PED_ID()) {
		float y_offset = 0;
		if (PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()) != 0) {
			Vector3 min, max, result;
			MISC::GET_MODEL_DIMENSIONS(hash, &min, &max);
			result = max - min;
			y_offset = result.y;
		}
		else if (!spawn_inside) {
			y_offset = 5.f;
		}
		return ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, Vector3(0.f, y_offset, 0.f));
	}
	inline void bring(Vehicle veh, Vector3 location, bool put_in = true, int seatIdx = -1) {
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh))
			return MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), TRANSLATE("INVALID_VEHICLE"));
		auto vecVehicleLocation = ENTITY::GET_ENTITY_COORDS(veh, true);
		load_ground_at_3dcoord(vecVehicleLocation);
		if (!take_control_of(veh))
			return MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_SELECTED_PLAYER_TELEPORT_OPTIONS"), TRANSLATE("FAILED_TO_TAKE_CONTROL_OF_VEHICLE"));
		auto ped = PLAYER::PLAYER_PED_ID();
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(veh, Vector3(location.x, location.y, location.z + 1.f), FALSE, FALSE, FALSE);
		ENTITY::SET_ENTITY_HEADING(veh, ENTITY::GET_ENTITY_HEADING(ped));
		if (put_in) {
			for (size_t i = 0; i < 100 && math::distance_between_vectors(location, ENTITY::GET_ENTITY_COORDS(veh, true)) > 10; i++)
				script::get_current()->yield();
			if (auto driver_ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, -1, false); driver_ped != 0) {
				if (PED::GET_PED_TYPE(driver_ped) == ePedType::PED_TYPE_NETWORK_PLAYER) {
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(driver_ped);
				}
				else {
					delete_entity(driver_ped);
				}
			}
			PED::SET_PED_INTO_VEHICLE(ped, veh, seatIdx);
		}
	}
	inline bool set_plate(Vehicle veh, const char* plate) {
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh) || !take_control_of(veh)) {
			return false;
		}
		if (plate != nullptr && plate[0] != 0) {
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh, plate);
		}
		return true;
	}
	inline bool repair(Vehicle veh) {
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh) || !take_control_of(veh, 0)) {
			return false;
		}
		VEHICLE::SET_VEHICLE_FIXED(veh);
		VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(veh);
		VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
		return true;
	}
}