#pragma once
#include "NetGameEvent.hpp"
#include "Enums.hpp"
#include "ScriptGlobal.hpp"
#include "Protections.hpp"
#include "GTAV-Classes-master/script/globals/GPBD_FM_3.hpp"
#include "GTAV-Classes-master/script/globals/GlobalPlayerBD.hpp"
namespace base //need to debug :(
{
	inline bool is_player_driver_of_local_vehicle(Player sender)
	{
		auto plyr = g_player_service->get_by_id(sender);

		if (!plyr || !plyr->get_current_vehicle() || !g_player_service->get_self()->get_current_vehicle())
			return false;

		return g_player_service->get_self()->get_current_vehicle()->m_driver == plyr->get_ped();
	}

	inline bool is_player_our_goon(Player sender)
	{
		auto& boss_goon = globals(1895156).as<GPBD_FM_3*>()->Entries[PLAYER::PLAYER_ID()].BossGoon;

		if (boss_goon.Boss != PLAYER::PLAYER_ID())
			return false;

		for (int i = 0; i < boss_goon.Goons.Size; i++)
		{
			if (boss_goon.Goons[i] == sender)
			{
				return true;
			}
		}

		return false;
	}

	inline bool is_player_our_boss(Player sender)
	{
		return sender == globals(1895156).as<GPBD_FM_3*>()->Entries[PLAYER::PLAYER_ID()].BossGoon.Boss;
	}

	bool scripted_game_eventHk(CScriptedGameEvent* scripted_game_event, CNetGamePlayer* player)
	{
		const auto args = scripted_game_event->m_args;

		const auto hash = static_cast<eScriptEvents>(args[0]);

		switch (hash)
		{
		case eScriptEvents::Bounty: {
			if (args[2] == PLAYER::PLAYER_ID())
			{
				auto togType = protections::g_blockBounty.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_BOUNTY_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_BOUNTY_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						const size_t arg_count = 22;
						int64_t args[arg_count] = { (int64_t)eScriptEvents::Bounty,
							PLAYER::PLAYER_ID(),
							player->m_player_id,
							1,
							10000,
							0,
							1,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							*globals(1924276).at(9).as<int*>(),
							*globals(1924276).at(10).as<int*>() };
						g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::CeoKick: {
			if (player->m_player_id != globals(1895156).as<GPBD_FM_3*>()->Entries[PLAYER::PLAYER_ID()].BossGoon.Boss)
			{
				auto togType = protections::g_blockCEOKick.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_CEO_KICK_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_CEO_KICK_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						auto leader = globals(1895156).as<GPBD_FM_3*>()->Entries[player->m_player_id].BossGoon.Boss;
						if (leader == -1)
							return;
						else if (leader == player->m_player_id) {
							const size_t arg_count = 2;
							int64_t args[arg_count] = { (int64_t)eScriptEvents::CeoKick, (int64_t)PLAYER::PLAYER_ID() };
							g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
						}
						else {
							// use a more private method to remove associate
							const size_t arg_count = 3;
							int64_t args[arg_count] = { (int64_t)eScriptEvents::MarkPlayerAsBeast, (int64_t)PLAYER::PLAYER_ID(), leader };
							g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
						}
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::CeoMoney: {
			if (player->m_player_id != globals(1895156).as<GPBD_FM_3*>()->Entries[PLAYER::PLAYER_ID()].BossGoon.Boss)
			{
				auto togType = protections::g_blockCEOMoney.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_CEO_MONEY_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_CEO_MONEY_FROM_PLAYER"), player->GetName());
					/*const size_t arg_count = 2;
					int64_t args[arg_count] = { (int64_t)eScriptEvents::CeoMoney, (int64_t)PLAYER::PLAYER_ID() };
					g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);*/
					return true;
				}
			}
			break;
		}
		case eScriptEvents::ClearWantedLevel: {
			if (!is_player_driver_of_local_vehicle(player->m_player_id))
			{
				auto togType = protections::g_blockClearWanted.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_CLEAR_WANTED_LVL_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_CLEAR_WANTED_LVL_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						constexpr size_t arg_count = 3;
						int64_t args[arg_count] = { static_cast<int64_t>(eScriptEvents::ClearWantedLevel),
							   PLAYER::PLAYER_ID(),
							   (int64_t)globals(1895156).as<GPBD_FM_3*>()->Entries[player->m_player_id].ScriptEventReplayProtectionCounter };
						g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::Kick3:
		case eScriptEvents::Kick4:
		case eScriptEvents::Kick5:
		case eScriptEvents::Kick6:
		case eScriptEvents::Kick7:
		case eScriptEvents::Kick8:
		case eScriptEvents::Kick9:
		case eScriptEvents::Kick10:
		case eScriptEvents::Kick11:
		case eScriptEvents::Kick12:
		case eScriptEvents::Kick13:
		case eScriptEvents::Kick14:
		case eScriptEvents::Kick15:
		case eScriptEvents::Kick16:
		case eScriptEvents::Kick2: {
			auto togType = protections::g_blockKick.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_TSE_KICKS"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_TSE_KICKS"), player->GetName());
				return true;
			}
		}
		case eScriptEvents::Crash: {
			auto togType = protections::g_blockCrashes.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_TSE_CRAHES"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_TSE_CRAHES"), player->GetName());
				Toxic::NullFunctionKick(g_player_service->get_by_id(player->m_player_id));
				Toxic::BailKick(g_player_service->get_by_id(player->m_player_id));
				Toxic::EndSessionKick(g_player_service->get_by_id(player->m_player_id));
				if (!player->IsNetworkHost()) {
					DesyncKickType = 0;
					Toxic::DesyncKick(player);
				}
				return true;
			}
			break;
		}
		case eScriptEvents::Crash2:
			if (args[2] > 32) // actual crash condition is if args[2] is above 255
			{
				auto togType = protections::g_blockCrashes.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_TSE_CRAHES"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_TSE_CRAHES"), player->GetName());
					Toxic::NullFunctionKick(g_player_service->get_by_id(player->m_player_id));
					Toxic::BailKick(g_player_service->get_by_id(player->m_player_id));
					Toxic::EndSessionKick(g_player_service->get_by_id(player->m_player_id));
					if (!player->IsNetworkHost()) {
						DesyncKickType = 0;
						Toxic::DesyncKick(player);
					}
					return true;
				}
			}
			break;
		case eScriptEvents::Crash3:
		{
			if (isnan(*(float*)&args[3]) || isnan(*(float*)&args[4]))
			{
				auto togType = protections::g_blockCrashes.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_TSE_CRAHES"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_TSE_CRAHES"), player->GetName());
					Toxic::NullFunctionKick(g_player_service->get_by_id(player->m_player_id));
					Toxic::BailKick(g_player_service->get_by_id(player->m_player_id));
					Toxic::EndSessionKick(g_player_service->get_by_id(player->m_player_id));
					if (!player->IsNetworkHost()) {
						DesyncKickType = 0;
						Toxic::DesyncKick(player);
					}
					return true;
				}
			}
			break;
		}
		case eScriptEvents::Notification: {
			switch (static_cast<eScriptEvents>(args[2]))
			{
			case eScriptEvents::NotificationMoneyBanked:
			case eScriptEvents::NotificationMoneyRemoved:
			case eScriptEvents::NotificationMoneyStolen:
				auto togType = protections::g_blockFakeDeposit.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FAKE_NOTIFY_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FAKE_NOTIFY_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						int64_t crashevent1[3] = { -642704387, 1, 0 };
						g_pointers->m_triggerScriptEvent(1, crashevent1, 3, 1 << player->m_player_id);
						});
					return true;
				}
				break;
			}
			break;
		}
		case eScriptEvents::ForceMission: {
			auto togType = protections::g_blockForceMission.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FORCE_TO_MISSION_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FORCE_TO_MISSION_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					int64_t FTM[3] = { 259469385, 1, 0 };
					g_pointers->m_triggerScriptEvent(1, FTM, 3, 1 << player->m_player_id);
					});
				return true;
			}
			break;
		}
		case eScriptEvents::GiveCollectible: {
			auto togType = protections::g_blockGiveCollectible.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_GIVE_COLLECTIBLE_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_GIVE_COLLECTIBLE_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					const size_t arg_count = 7;
					eCollectibleType col{};
					int index = 0;
					bool uncomplete = false;
					std::int64_t args[arg_count] = {
						(int64_t)eScriptEvents::GiveCollectible,
						(int64_t)PLAYER::PLAYER_ID(),
						(int64_t)col,   // iParam0
						(int64_t)index, // iParam1
						!uncomplete,    // bParam2
						true,
						0 // bParam3
					};
					g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
					});
				return true;
			}
			break;
		}
		case eScriptEvents::GtaBanner: {
			auto togType = protections::g_blockGTABanner.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_GTA_BANNER_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_GTA_BANNER_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					int64_t GTB[3] = { -330501227, 1, 0 };
					g_pointers->m_triggerScriptEvent(1, GTB, 3, 1 << player->m_player_id);
					});
				return true;
			}
			break;
		}
		case eScriptEvents::MCTeleport: {
			if (args[3] <= 32 && !is_player_our_boss(player->m_player_id))
			{
				for (int i = 0; i < 32; i++)
				{
					if (args[4 + i] == NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(PLAYER::PLAYER_ID()))
					{
						auto togType = protections::g_blockMCTeleport.toggleType;
						if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
							MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_MC_TELEPORT_FROM_PLAYER"), player->GetName());
						}
						if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
							return true;
						}
						if (togType == eToggleTypes::tog_notifyBlockandKarma)
						{
							MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_GTA_BANNER_FROM_PLAYER"), player->GetName());
							g_fiber_pool->queue_job([&] {
								int64_t MCT[3] = { 1103127469, 1, 0 };
								g_pointers->m_triggerScriptEvent(1, MCT, 3, 1 << player->m_player_id);
								});
							return true;
						}
					}
				}
			}
			else if (args[3] > 32)
			{
				auto togType = protections::g_blockMCTeleport.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_MC_TELEPORT_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_MC_TELEPORT_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						int64_t MCT[3] = { 1103127469, 1, 0 };
						g_pointers->m_triggerScriptEvent(1, MCT, 3, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::PersonalVehicleDestroyed: {
			auto togType = protections::g_blockPersonalVehDestory.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_PERSONAL_VEH_DESTROY_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_PERSONAL_VEH_DESTROY_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					int64_t PD[3] = { 1655503526, 1, 0 };
					g_pointers->m_triggerScriptEvent(1, PD, 3, 1 << player->m_player_id);
					});
				return true;
			}
			break;
		}
		case eScriptEvents::RemoteOffradar: {
			if (!is_player_our_boss(player->m_player_id) && !is_player_driver_of_local_vehicle(player->m_player_id))
			{
				auto togType = protections::g_blockRemoteOffRadar.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_REMOTE_OFF_RADAR_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_REMOTE_OFF_RADAR_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						int64_t ROR[3] = { 57493695, 1, 0 };
						g_pointers->m_triggerScriptEvent(1, ROR, 3, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::TSECommandRotateCam:
		case eScriptEvents::TSECommand: {
			if (static_cast<eScriptEvents>(args[2]) == eScriptEvents::TSECommandRotateCam && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
			{
				auto togType = protections::g_blockRotateCam.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_ROTATE_CAM_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_ROTATE_CAM_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						int64_t TSECR[3] = { 800157557, 1, 0 };
						g_pointers->m_triggerScriptEvent(1, TSECR, 3, 1 << player->m_player_id);
						int64_t TSEC[3] = { 225624744, 1, 0 };
						g_pointers->m_triggerScriptEvent(1, TSEC, 3, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::SendToCutscene: {
			if (!is_player_our_boss(player->m_player_id))
			{
				auto togType = protections::g_blockSendToCutscene.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SEND_TO_CUTSCENE_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SEND_TO_CUTSCENE_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						int64_t STC[3] = { -1951335381, 1, 0 };
						g_pointers->m_triggerScriptEvent(1, STC, 3, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::SendToLocation:
		{
			if (is_player_our_boss(player->m_player_id))
				break;

			bool known_location = false;

			if (args[2] == 0 && args[3] == 0)
			{
				if (args[4] == 4 && args[5] == 0)
				{
					known_location = true;

					auto togType = protections::g_blockSendToLocation.toggleType;
					if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SEND_TO_LOCATION_FROM_PLAYER"), player->GetName());
					}
					if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
						return true;
					}
					if (togType == eToggleTypes::tog_notifyBlockandKarma)
					{
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SEND_TO_LOCATION_FROM_PLAYER"), player->GetName());
						g_fiber_pool->queue_job([&] {
							int64_t STL[3] = { -1951335381, 1, 0 };
							g_pointers->m_triggerScriptEvent(1, STL, 3, 1 << player->m_player_id);
							});
						return true;
					}
				}
				else if ((args[4] == 3 || args[4] == 4) && args[5] == 1)
				{
					known_location = true;

					auto togType = protections::g_blockSendToLocation.toggleType;
					if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SEND_TO_LOCATION_FROM_PLAYER"), player->GetName());
					}
					if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
						return true;
					}
					if (togType == eToggleTypes::tog_notifyBlockandKarma)
					{
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SEND_TO_LOCATION_FROM_PLAYER"), player->GetName());
						g_fiber_pool->queue_job([&] {
							int64_t STL[3] = { -1951335381, 1, 0 };
							g_pointers->m_triggerScriptEvent(1, STL, 3, 1 << player->m_player_id);
							});
						return true;
					}
				}
			}
			if (!known_location)
			{
				auto togType = protections::g_blockTSEFreeze.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FREEZE_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FREEZE_FROM_PLAYER"), player->GetName());
					g_pointers->m_clearPedTasksNetwork(player->m_player_info->m_ped, true);
					return true;
				}
			}
			break;
		}
		case eScriptEvents::Spectate:
		{
			auto togType = protections::g_blockSpectate.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SPECTATE_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SPECTATE_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					int64_t SP[3] = { 1840946429, 1, 0 };
					g_pointers->m_triggerScriptEvent(1, SP, 3, 1 << player->m_player_id);
					});
				return true;
			}
			break;
		}
		case eScriptEvents::Teleport: {
			if (!is_player_driver_of_local_vehicle(player->m_player_id))
			{
				auto togType = protections::g_blockForceTeleport.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FORCE_TELEPORT_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FORCE_TELEPORT_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						int64_t TP[3] = { -1321657966, 1, 0 };
						g_pointers->m_triggerScriptEvent(1, TP, 3, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::TransactionError:
		{
			auto togType = protections::g_blockTransactionErr.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_TRANSACTION_ERROR_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_TRANSACTION_ERROR_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					const size_t arg_count = 8;
					int64_t args[arg_count] = { (int64_t)eScriptEvents::TransactionError,
						(int64_t)PLAYER::PLAYER_ID(),
						1,
						0,
						0,
						globals(1895156).as<GPBD_FM_3*>()->Entries[player->m_player_id].ScriptEventReplayProtectionCounter,
						*globals(1924276).at(9).as<int*>(),
						*globals(1924276).at(10).as<int*>() };
					g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
					});
				return true;
			}
			break;
		}
		case eScriptEvents::VehicleKick:
		{
			auto togType = protections::g_blockVehKick.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_VEHICLE_KICK_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_VEHICLE_KICK_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					auto vehicle = player->m_player_info->m_ped->m_vehicle;
					if (!vehicle || !vehicle->m_net_object) {
						// vehicle hasn't synced yet, use TSE
						const size_t arg_count = 9;
						int64_t args[arg_count] = { (int64_t)eScriptEvents::VehicleKick, PLAYER::PLAYER_ID(), 0, 0, 0, 0, 0, 0, 0 };
						g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
					}
					else
					{
						// use a private method to kick player from vehicle
						(*g_pointers->m_network_object_mgr)
							->ChangeOwner(vehicle->m_net_object, g_player_service->get_self()->get_net_game_player(), 0);
					}
					});
				return true;
			}
			break;
		}
		case eScriptEvents::NetworkBail:
		{
			auto togType = protections::g_blockNetBail.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("NETWORKBAIL_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				g_player_service->get_by_id(player->m_player_id)->is_modder = true;
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				g_player_service->get_by_id(player->m_player_id)->is_modder = true;
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("NETWORKBAIL_FROM_PLAYER"), player->GetName());
				Toxic::BailKick(g_player_service->get_by_id(player->m_player_id));
				return true;
			}
			break;
		}
		case eScriptEvents::TeleportToWarehouse: {
			if (!is_player_driver_of_local_vehicle(player->m_player_id))
			{
				auto togType = protections::g_blockTeleportToWarehouse.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_TELEPORT_TO_WAREHOUSE"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_TELEPORT_TO_WAREHOUSE"), player->GetName());
					g_fiber_pool->queue_job([&] {
						const size_t arg_count = 6;
						const std::vector<std::uint64_t>& _args{};
						int64_t args[arg_count] = { (int64_t)eScriptEvents::TeleportToWarehouse, PLAYER::PLAYER_ID(), (int64_t)player->m_player_id, 1, (int64_t)_args[0] };
						g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::StartActivity: {
			eActivityType activity = static_cast<eActivityType>(args[2]);

			auto togType = protections::g_blockStartActivity.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_START_ACTIVITY_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				if (activity == eActivityType::Survival || activity == eActivityType::Mission || activity == eActivityType::Deathmatch || activity == eActivityType::BaseJump || activity == eActivityType::Race)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_FREEZE_FROM_PLAYER"), player->GetName());
					return true;
				}
				else if (activity == eActivityType::Darts)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_START_ACTIVITY_DARTS_FROM_PLAYER"), activity, player->GetName());
					return true;
				}
				else if (activity == eActivityType::PilotSchool)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_START_ACTIVITY_PILOT_SCHOOL_FROM_PLAYER"), activity, player->GetName());
					return true;
				}
				else if (activity == eActivityType::ImpromptuDeathmatch)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_START_ACTIVITY_IMPR_DEATHMATCH_FROM_PLAYER"), activity, player->GetName());
					return true;
				}
				else if (activity == eActivityType::DefendSpecialCargo || activity == eActivityType::GunrunningDefend || activity == eActivityType::BikerDefend || args[2] == 238)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_START_ACTIVITY_TRIGGER_BUSINESS_RAID_FROM_PLAYER"), activity, player->GetName());
					return true;
				}
			}
			else if (activity == eActivityType::Tennis)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_START_ACTIVITY_TENNIS_FROM_PLAYER"), activity, player->GetName());
				return true;
			}
			if (!is_player_our_goon(player->m_player_id))
			{
				auto togType = protections::g_blockTeleportToWarehouse.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_TELEPORT_TO_WAREHOUSE"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
			}
			break;
		}
		case eScriptEvents::InteriorControl: {
			auto interior = (int)args[2];
			if (interior < 0 || interior > 161)  //the upper bound will change after an update
			{
				if (auto plyr = g_player_service->get_by_id(player->m_player_id))
				{
					auto togType = protections::g_blockInteriorControl.toggleType;
					if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_INTERIOR_CONTROL_FROM_PLAYER"), player->GetName());
					}
					if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
						return true;
					}
					if (togType == eToggleTypes::tog_notifyBlockandKarma)
					{
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_INTERIOR_CONTROL_FROM_PLAYER"), player->GetName());
						Toxic::NullFunctionKick(plyr);
						return true;
					}
				}
				auto togType = protections::g_blockNullFunctionKick.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_NULL_FUNCTION_KICK_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_NULL_FUNCTION_KICK_FROM_PLAYER"), player->GetName());
					Toxic::NullFunctionKick(g_player_service->get_by_id(player->m_player_id));
					return true;
				}
			}
			if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
				break;

			if (!gta_util::get_local_ped())
				break;

			if (is_player_our_boss(player->m_player_id))
				break;

			if (is_player_driver_of_local_vehicle(player->m_player_id))
				break;

			if (!player->m_player_info->m_ped || math::distance_between_vectors(*player->m_player_info->m_ped->GetNavigation()->get_position(), *gta_util::get_local_ped()->get_position()) > 75.0f)
			{
				auto togType = protections::g_blockSendToInterior.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SEND_TO_INTERIOR_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SEND_TO_INTERIOR_FROM_PLAYER"), player->GetName());
					Toxic::NullFunctionKick(g_player_service->get_by_id(player->m_player_id));
					return true;
				}
			}
			break;
		}
		case eScriptEvents::DestroyPersonalVehicle: {
			auto togType = protections::g_blockPersonalVehDestory.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_PERSONAL_VEH_DESTROY_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_PERSONAL_VEH_DESTROY_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					int64_t DPV[3] = { 109434679, 1, 0 };
					g_pointers->m_triggerScriptEvent(1, DPV, 3, 1 << player->m_player_id);
					});
				return true;
			}
			break;
		}
		case eScriptEvents::KickFromInterior: {
			if (globals(2657704).as<GlobalPlayerBD*>()->Entries[PLAYER::PLAYER_ID()].SimpleInteriorData.Owner != player->m_player_id)
			{
				auto togType = protections::g_blockKickFromInterior.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_KICK_FROM_INTERIOR_FROM_PLAYER"), player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return true;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_KICK_FROM_INTERIOR_FROM_PLAYER"), player->GetName());
					g_fiber_pool->queue_job([&] {
						const size_t arg_count = 8;
						int64_t args[arg_count]{
							(int64_t)eScriptEvents::KickFromInterior,
							(int64_t)PLAYER::PLAYER_ID(),
							(int64_t)globals(2657704).as<GlobalPlayerBD*>()
								->Entries[player->m_player_id]
								.SimpleInteriorData.Index,
							(int64_t)globals(2657704).as<GlobalPlayerBD*>()
								->Entries[player->m_player_id]
								.SimpleInteriorData.InstanceId,
						};
						g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
						});
					return true;
				}
			}
			break;
		}
		case eScriptEvents::TriggerCEORaid: {
			auto togType = protections::g_blockTriggerCeoRaid.toggleType;
			if (auto script = gta_util::find_script_thread("freemode"_joaat))
			{
				if (script->m_net_component && ((rage::CGameScriptHandlerNetComponent*)script->m_net_component)->m_host && ((rage::CGameScriptHandlerNetComponent*)script->m_net_component)->m_host->m_net_game_player != player)
				{
					if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_TRIGGER_CEO_RAID_FROM_PLAYER"), player->GetName());
					}
					if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
						return true;
					}
					if (togType == eToggleTypes::tog_notifyBlockandKarma)
					{
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_TRIGGER_CEO_RAID_FROM_PLAYER"), player->GetName());
						g_fiber_pool->queue_job([&] {
							const size_t arg_count = 3;
							int64_t args[arg_count] = { (int64_t)eScriptEvents::TriggerCEORaid, (int64_t)PLAYER::PLAYER_ID(), 0 };
							g_pointers->m_triggerScriptEvent(1, args, arg_count, 1 << player->m_player_id);
							});
						return true;
					}
				}
			}
			break;
		}
		case eScriptEvents::StartScriptProceed: {
			//TODO: Breaks stuff
			if (auto script = gta_util::find_script_thread("freemode"_joaat))
			{
				if (script->m_net_component && ((rage::CGameScriptHandlerNetComponent*)script->m_net_component)->m_host && ((rage::CGameScriptHandlerNetComponent*)script->m_net_component)->m_host->m_net_game_player != player)
				{
					auto togType = protections::g_blockStartScript.toggleType;
					if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_START_SCRIPT_PROCEED"), player->GetName());
					}
					if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
						return true;
					}
					if (togType == eToggleTypes::tog_notifyBlockandKarma)
					{
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_START_SCRIPT_PROCEED"), player->GetName());
						g_fiber_pool->queue_job([&] {
							int64_t DPV[3] = { 1757622014, 1, 0 };
							g_pointers->m_triggerScriptEvent(1, DPV, 3, 1 << player->m_player_id);
							});
						return true;
					}
				}
			}
			break;
		}
		case eScriptEvents::SoundSpam: {
			auto togType = protections::g_blockSoundSpam.toggleType;
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SOUND_SPAM_FROM_PLAYER"), player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("TSE_SOUND_SPAM_FROM_PLAYER"), player->GetName());
				g_fiber_pool->queue_job([&] {
					int64_t SP[3] = { 996099702, 1, 0 };
					g_pointers->m_triggerScriptEvent(1, SP, 3, 1 << player->m_player_id);
					});
				return true;
			}
			break;
		}
		}

		return false;
	}
}