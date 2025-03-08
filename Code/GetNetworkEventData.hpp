#include "NetGameEvent.hpp"
#include "Hooking.hpp"
#include "Services/Player/player_service.hpp"
#include "GTAV-Classes-master/entities/CDynamicEntity.hpp"
#include "Notify.hpp"
#include "Translations.hpp"
#include "GtaUtill.hpp"
#include "GTAV-Classes-master/script/globals/GPBD_FM.hpp"
#include "TaskJumpAndFallConstructor.hpp"
#include "MainScript.hpp"
#include "Services/Api/api_service.hpp"
namespace base
{
	inline bool is_invincible(base::player_ptr player)
	{
		return player->get_ped() && (player->get_ped()->m_damage_bits & (1 << 8));
	}
	inline bool is_invisible(base::player_ptr player)
	{
		if (!player->get_ped())
			return false;

		if (!NETWORK::NETWORK_ARE_PLAYERS_IN_SAME_TUTORIAL_SESSION(PLAYER::PLAYER_ID(), player->id()))
			return false; // probably not

		if (globals(2657704).as<GlobalPlayerBD*>()->Entries[player->id()].IsInvisible)
			return true;

		if ((player->get_current_vehicle() && player->get_current_vehicle()->m_driver == player->get_ped())
			|| PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(player->id()))
			return false; // can't say

		return false; // TODO! have to get data from CPhysicalGameStateDataNode
		//return (player->get_ped()->m_flags & (int)rage::fwEntity::EntityFlags::IS_VISIBLE) == 0;
	}
	inline bool is_hidden_from_player_list(base::player_ptr player)
	{
		return globals(2657704).as<GlobalPlayerBD*>()->Entries[player->id()].CayoPericoFlags & 1;
	}
	inline bool is_using_rc_vehicle(base::player_ptr player)
	{
		if (Bits::HasBitsSet(&globals(1853988).as<GPBD_FM*>()->Entries[player->id()].PropertyData.PAD_0365, 29))
			return true; // bandito

		if (Bits::HasBitsSet(&globals(1853988).as<GPBD_FM*>()->Entries[player->id()].PropertyData.ArcadeData.AppearanceBitset2, 16))
			return true; // tank

		return false;
	}
	inline bool is_using_orbital_cannon(base::player_ptr player)
	{
		return globals(2657704).as<GlobalPlayerBD*>()->Entries[player->id()].OrbitalBitset.IsSet(eOrbitalBitset::kOrbitalCannonActive);
	}

	void hooks::get_network_event_data(int64_t unk, rage::CEventNetwork* net_event)
	{
		switch (net_event->get_type())
		{
		case rage::eEventNetworkType::CEventNetworkRemovedFromSessionDueToComplaints:
		{
			if (features::rejoin_kicked_session && !NETWORK::NETWORK_IS_ACTIVITY_SESSION() && SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) == 0 && !STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
			{
				g_fiber_pool->queue_job([] {
					g_api_service->join_session(gta_util::get_network()->m_last_joined_session.m_session_info);
					});
				MainNotification(ImGuiToastType_Warning, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("USER_DESYNC_KICKED_JOIN_BACK"));
			}
			else
			{
				MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("USER_DESYNC_KICKED"));
			}
			break;
		}
		case rage::eEventNetworkType::CEventNetworkPlayerCollectedAmbientPickup:
		{
			rage::sEventPlayerCollectedAmbientPickup playerCollectedAmbientPickup{};
			net_event->get_extra_information(&playerCollectedAmbientPickup, sizeof(playerCollectedAmbientPickup));
			if (auto player = gta_util::getCNetGamePlayerViaPlayerIndex(playerCollectedAmbientPickup.m_player_index)) {
				static ankerl::unordered_dense::set<rage::joaat_t> kickPickupModels = {
					"vw_prop_vw_colle_alien"_joaat,
					"vw_prop_vw_colle_imporage"_joaat,
					"vw_prop_vw_colle_beast"_joaat,
					"vw_prop_vw_colle_pogo"_joaat,
					"vw_prop_vw_colle_prbubble"_joaat,
					"vw_prop_vw_colle_rsrcomm"_joaat,
					"vw_prop_vw_colle_rsrgeneric"_joaat,
					"vw_prop_vw_colle_sasquatch"_joaat
				};
				static ankerl::unordered_dense::set<rage::joaat_t> highAmountPickupModels = {
					"vw_prop_vw_lux_card_01a"_joaat
				};
				if (kickPickupModels.count(playerCollectedAmbientPickup.m_pickup_model) > 0)
				{
					//MainNotification(ImGuiToastType_Info, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("MODDER_PU_KICK_DETECT"), player->GetName());
					if (playerCollectedAmbientPickup.m_pickup_amount > 10)
					{
						g_player_service->get_by_id(player->m_player_id)->is_modder = true;
						//MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("KICK_PU0_FROM_PLAYER"), player->GetName(), playerCollectedAmbientPickup.m_pickup_type, playerCollectedAmbientPickup.m_pickup_amount, playerCollectedAmbientPickup.m_pickup_index, playerCollectedAmbientPickup.m_pickup_model, playerCollectedAmbientPickup.m_player_index);
						playerCollectedAmbientPickup.m_pickup_amount = 2;
						return;
					}
				}
				else if (highAmountPickupModels.count(playerCollectedAmbientPickup.m_pickup_model) > 0)
				{
					//MainNotification(ImGuiToastType_Info, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("MODDER_PU_KICK_DETECT"), player->GetName());
					if (playerCollectedAmbientPickup.m_pickup_amount > 2000)
					{
						g_player_service->get_by_id(player->m_player_id)->is_modder = true;
						//MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("KICK_PU1_FROM_PLAYER"), player->GetName());
						playerCollectedAmbientPickup.m_pickup_amount = 2;
						return;
					}
				}
			}
		}	break;
		case rage::eEventNetworkType::CEventNetworkEntityDamage:
		{
			rage::sEntityDamagedData damage_data;
			net_event->get_extra_information(&damage_data, sizeof(damage_data));

			if (damage_data.m_weapon_used == "WEAPON_STICKYBOMB"_joaat || damage_data.m_weapon_used == "VEHICLE_WEAPON_MINE_KINETIC_RC"_joaat
				|| damage_data.m_weapon_used == "VEHICLE_WEAPON_MINE_EMP_RC"_joaat || damage_data.m_weapon_used == "VEHICLE_WEAPON_MINE_KINETIC"_joaat
				|| damage_data.m_weapon_used == "VEHICLE_WEAPON_MINE_EMP"_joaat || damage_data.m_weapon_used == "VEHICLE_WEAPON_MINE_SPIKE"_joaat)
				break;

			if (auto damager = g_pointers->m_handleToPointer(damage_data.m_damager_index);
				damager && damager->m_entity_type == 4 && reinterpret_cast<CPed*>(damager)->m_player_info)
			{
				if (auto player = g_player_service->get_by_host_token(
					reinterpret_cast<CPed*>(damager)->m_player_info->m_net_player_data.m_host_token))
				{
					if (PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(player->id()))
					{
						if (globals(2657704).as<GlobalPlayerBD*>()->Entries[player->id()].PlayerBlip.PlayerVehicleBlipType == eBlipType::SUBMARINE)
							break;

						if (is_using_rc_vehicle(player))
							break;
					}
					else
					{
						if (auto vehicle = player->get_current_vehicle())
							if (auto model_info = vehicle->m_model_info)
								if (model_info->m_hash == "rcbandito"_joaat || model_info->m_hash == "minitank"_joaat
									|| model_info->m_hash == "kosatka"_joaat)
									break;
					}

					if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
						break;

					if (!NETWORK::NETWORK_ARE_PLAYERS_IN_SAME_TUTORIAL_SESSION(PLAYER::PLAYER_ID(), player->id()))
						break;

					if (g_mainScript.get_interior_from_player(player->id()) != 0)
						break;

					if (player->get_player_info() && player->get_player_info()->m_game_state == eGameState::InMPCutscene)
						break;

					if (auto victim = g_pointers->m_handleToPointer(damage_data.m_victim_index); victim && victim->m_entity_type == 4)
					{
						if (is_invincible(player))
						{
							player->is_modder = true;
							MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("ATTACKING_WITH_GODMODE"), player->get_name());
						}

						if (is_invisible(player))
						{
							if (!reinterpret_cast<CPed*>(victim)->m_player_info)
								break;

							if (damage_data.m_weapon_used == "WEAPON_EXPLOSION"_joaat || damage_data.m_weapon_used == "WEAPON_RAMMED_BY_CAR"_joaat
								|| damage_data.m_weapon_used == "WEAPON_RUN_OVER_BY_CAR"_joaat)
								break;

							player->is_modder = true;
							MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("ATTACKING_WITH_INVISIBLE"), player->get_name());
						}

						if (is_hidden_from_player_list(player))
						{
							player->is_modder = true;
							MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("ATTACKING_WHEN_HIDDEN_FROM_PLAYER_LIST"), player->get_name());
						}

						if (is_using_orbital_cannon(player))
						{
							player->is_modder = true;
							MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("SPOOFED_DATA"), player->get_name());
						}
					}
				}
			}
			break;
		}
		case rage::eEventNetworkType::CEventNetworkBail:
		{
			MainNotification(ImGuiToastType_Info, 7000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("NET_BAIL_WAS_CALLED"));
			break;
		}
		}

		return g_hooking->m_getNetworkEventDataHk.getOg<decltype(&get_network_event_data)>()(unk, net_event);
	}
}