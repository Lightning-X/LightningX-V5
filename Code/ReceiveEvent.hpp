#include "Fibers/fiber_pool.hpp"
#include "Enums.hpp"
#include "NetGameEvent.hpp"
#include "Hooking.hpp"
#include "Math.hpp"
#include "Notify.hpp"
#include "Translations.hpp"
#include "KicksAndCrashes.hpp"
#include "GtaUtill.hpp"
#include "GTAV-Classes-master/base/CObject.hpp"
#include "GTAV-Classes-master/network/CNetGamePlayer.hpp"
#include "Features.hpp"
#include "Protections.hpp"
#include "ScriptedGameEvent.hpp"
#include "Entity.hpp"
#include "HashList.hpp"
namespace base
{
	bool incrementStatEvent(CNetworkIncrementStatEvent* ise, CNetGamePlayer* sender) {
		for (auto const& evnt : protections::m_reportHashes) {
			if (ise->m_stat != evnt.hash) {
				continue;
			}
			auto togType = protections::m_blockReports.toggleType;
			if (togType == eToggleTypes::tog_none) {
				return false;
			}
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				g_logger->send(logColor::red, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REPORT_FROM_PLAYER"), evnt.name, sender->GetName());
				MainNotification(ImGuiToastType_Protections, 10000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REPORT_FROM_PLAYER"), evnt.name, sender->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return true;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma) {
				g_logger->send(logColor::red, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REPORT_FROM_PLAYER"), evnt.name, sender->GetName());
				MainNotification(ImGuiToastType_Protections, 10000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REPORT_FROM_PLAYER"), evnt.name, sender->GetName());
				// ADD Redirect, but for now will just kick :)
				DesyncKickType = 0;
				gta_util::get_network()->m_game_session.is_host() ? Toxic::HostKick(g_player_service->get_by_id(sender->m_player_id)) : Toxic::DesyncKick(sender);
				return true;
			}
		}
		return false;
	}
	inline Vehicle get_personal_vehicle() {
		return *globals(2794162).at(299).as<Vehicle*>();
	}
	void Redirect1945Moment(CNetGamePlayer* player) {
		auto loc = player->m_player_info->m_ped->GetNavigation()->get_position();
		while (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("core")) {
			STREAMING::REQUEST_NAMED_PTFX_ASSET("core");
			script::get_current()->yield();
		}
		GRAPHICS::USE_PARTICLE_FX_ASSET("core");
		GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD("veh_respray_smoke", Vector3(loc->x, loc->y, loc->z), Vector3(-0.5f, 0.0f, 0.0f), 10.0f, false, false, false, false);
		STREAMING::REMOVE_NAMED_PTFX_ASSET("core");
	}
	void scan_weapon_damage_event(CNetGamePlayer* player, rage::datBitBuffer* buffer)
	{
		uint8_t damageType;
		uint32_t weaponType; // weaponHash

		bool overrideDefaultDamage;
		bool hitEntityWeapon;
		bool hitWeaponAmmoAttachment;
		bool silenced;

		uint32_t damageFlags;
		bool hasActionResult;

		uint32_t actionResultName;
		uint16_t actionResultId;
		uint32_t f104;

		uint32_t weaponDamage;
		bool isNetTargetPos;

		rage::fvector3 localPos;

		bool f112;

		uint32_t damageTime;
		bool willKill;
		uint32_t f120;
		bool hasVehicleData;

		uint16_t f112_1;

		uint16_t parentGlobalId; // Source entity?
		uint16_t hitGlobalId;    // Target entity?

		uint8_t tyreIndex;
		uint8_t suspensionIndex;
		uint8_t hitComponent;

		bool f133;
		bool hasImpactDir;

		rage::fvector3 impactDir;

		damageType = buffer->Read<uint8_t>(2);
		weaponType = buffer->Read<uint32_t>(32);

		overrideDefaultDamage = buffer->Read<uint8_t>(1);
		hitEntityWeapon = buffer->Read<uint8_t>(1);
		hitWeaponAmmoAttachment = buffer->Read<uint8_t>(1);
		silenced = buffer->Read<uint8_t>(1);

		damageFlags = buffer->Read<uint32_t>(24);
		// (damageFlags >> 1) & 1
		hasActionResult = buffer->Read<uint8_t>(1);

		if (hasActionResult)
		{
			actionResultName = buffer->Read<uint32_t>(32);
			actionResultId = buffer->Read<uint16_t>(16);
			f104
				= buffer->Read<uint32_t>(32);
		}

		if (overrideDefaultDamage)
		{
			weaponDamage = buffer->Read<uint32_t>(17);
		}
		else
		{
			weaponDamage = 0;
		}

		bool _f92 = buffer->Read<uint8_t>(1);

		if (_f92)
		{
			buffer->Read<uint8_t>(4);
		}

		isNetTargetPos = buffer->Read<uint8_t>(1);

		if (isNetTargetPos)
		{
			localPos.x = buffer->ReadSignedFloat(16, 55.f); // divisor: 0x425C0000
			localPos.y = buffer->ReadSignedFloat(16, 55.f);
			localPos.z = buffer->ReadSignedFloat(16, 55.f);
		}

		if (damageType == 3)
		{
			damageTime = buffer->Read<uint32_t>(32);
			willKill = buffer->Read<uint8_t>(1);

			if (hasActionResult)
			{
				hitGlobalId = buffer->Read<uint16_t>(13);
			}
			else
			{
				hitGlobalId = 0;
			}

			f112 = buffer->Read<uint8_t>(1);

			if (!f112)
			{
				f112_1 = buffer->Read<uint16_t>(11);
			}
			else
			{
				f112_1 = buffer->Read<uint16_t>(16);
			}
		}
		else
		{
			parentGlobalId = buffer->Read<uint16_t>(13); // +118
			hitGlobalId = buffer->Read<uint16_t>(13); // +120
		}

		if (damageType < 2)
		{
			localPos.x = buffer->ReadSignedFloat(16, 55.f); // divisor: 0x425C0000
			localPos.y = buffer->ReadSignedFloat(16, 55.f);
			localPos.z = buffer->ReadSignedFloat(16, 55.f);

			if (damageType == 1)
			{
				hasVehicleData = buffer->Read<uint8_t>(1);

				if (hasVehicleData)
				{
					tyreIndex = buffer->Read<uint8_t>(4); // +122
					suspensionIndex = buffer->Read<uint8_t>(4); // +123
				}
			}
		}
		else
		{
			hitComponent = buffer->Read<uint8_t>(5); // +108
		}

		f133 = buffer->Read<uint8_t>(1);
		hasImpactDir = buffer->Read<uint8_t>(1);

		if (hasImpactDir)
		{
			impactDir.x = buffer->ReadSignedFloat(16, 6.2831854820251f); // divisor: 0x40C90FDB
			impactDir.y = buffer->ReadSignedFloat(16, 6.2831854820251f);
			impactDir.z = buffer->ReadSignedFloat(16, 6.2831854820251f);
		}

		buffer->Seek(0);

		// LOG(INFO) << localPos.x << " " << localPos.y << " " << localPos.z << ". " << math::distance_between_vectors(localPos, *g_local_player->m_navigation->get_position()) << " " << (g_local_player->m_net_object->m_object_id == hitGlobalId ? "TRUE" : "FALSE") << " " << damageType << " " << damageFlags;

		if (damageType == 3 && (damageFlags & (1 << 1)) == 0)
			hitGlobalId = gta_util::get_local_ped() ? gta_util::get_local_ped()->m_net_object->m_object_id : 0;

		if (gta_util::get_local_ped() && gta_util::get_local_ped()->m_net_object && (gta_util::get_local_ped()->m_net_object->m_object_id == hitGlobalId || math::distance_between_vectors(localPos, *gta_util::get_local_ped()->m_navigation->get_position()) < 1.5f))
		{
			int id = player->m_player_id;
			g_fiber_pool->queue_job([&id, &localPos, &hitComponent, &overrideDefaultDamage, &weaponType, &weaponDamage, &tyreIndex, &suspensionIndex, &damageFlags, &actionResultName, &actionResultId, &f104, &hitEntityWeapon, &hitWeaponAmmoAttachment, &silenced] {
				player_ptr Targetplayer = g_player_service->get_by_id(id);

				if (!Targetplayer->is_valid() || !Targetplayer->get_ped())
					return;

				auto togType = protections::g_blockDamageKarma.toggleType;
				if (togType == eToggleTypes::tog_none) {
					return;
				}
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_DAMAGE"), Targetplayer->get_name());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_DAMAGE"), Targetplayer->get_name());
					Redirect1945Moment(Targetplayer->get_net_game_player());
					g_pointers->m_send_network_damage(g_player_service->get_self()->get_ped(),
						Targetplayer->get_ped(),
						&localPos,
						hitComponent,
						overrideDefaultDamage,
						weaponType,
						weaponDamage,
						tyreIndex,
						suspensionIndex,
						damageFlags,
						actionResultName,
						actionResultId,
						f104,
						hitEntityWeapon,
						hitWeaponAmmoAttachment,
						silenced,
						false,
						Targetplayer->get_ped()->m_navigation->get_position());
				}
				});
		}
	}

	void scan_explosion_event(CNetGamePlayer* player, rage::datBitBuffer* buffer)
	{
		uint16_t f186;
		uint16_t f208;
		int ownerNetId;
		uint16_t f214;
		eExplosionTag explosionType;
		float damageScale;

		float posX;
		float posY;
		float posZ;

		bool f242;
		uint16_t f104;
		float cameraShake;

		bool isAudible;
		bool f189;
		bool isInvisible;
		bool f126;
		bool f241;
		bool f243;

		uint16_t f210;

		float unkX;
		float unkY;
		float unkZ;

		bool f190;
		bool f191;

		uint32_t f164;

		float posX224;
		float posY224;
		float posZ224;

		bool f240;
		uint16_t f218;
		bool f216;

		// clang-format off

		f186 = buffer->Read<uint16_t>(16);
		f208 = buffer->Read<uint16_t>(13);
		ownerNetId = buffer->Read<uint16_t>(13);
		f214 = buffer->Read<uint16_t>(13);               // 1604+
		explosionType = (eExplosionTag)buffer->ReadSigned<int>(8);// 1604+ bit size
		damageScale = buffer->Read<int>(8) / 255.0f;

		posX = buffer->ReadSignedFloat(22, 27648.0f);
		posY = buffer->ReadSignedFloat(22, 27648.0f);
		posZ = buffer->ReadFloat(22, 4416.0f) - 1700.0f;

		f242 = buffer->Read<uint8_t>(1);
		f104 = buffer->Read<uint16_t>(16);
		cameraShake = buffer->Read<int>(8) / 127.0f;

		isAudible = buffer->Read<uint8_t>(1);
		f189 = buffer->Read<uint8_t>(1);
		isInvisible = buffer->Read<uint8_t>(1);
		f126 = buffer->Read<uint8_t>(1);
		f241 = buffer->Read<uint8_t>(1);
		f243 = buffer->Read<uint8_t>(1);// 1604+

		f210 = buffer->Read<uint16_t>(13);

		unkX = buffer->ReadSignedFloat(16, 1.1f);
		unkY = buffer->ReadSignedFloat(16, 1.1f);
		unkZ = buffer->ReadSignedFloat(16, 1.1f);

		f190 = buffer->Read<uint8_t>(1);
		f191 = buffer->Read<uint8_t>(1);
		f164 = buffer->Read<uint32_t>(32);

		if (f242)
		{
			posX224 = buffer->ReadSignedFloat(31, 27648.0f);
			posY224 = buffer->ReadSignedFloat(31, 27648.0f);
			posZ224 = buffer->ReadFloat(31, 4416.0f) - 1700.0f;
		}
		else
		{
			posX224 = 0;
			posY224 = 0;
			posZ224 = 0;
		}

		auto f168 = buffer->Read<uint32_t>(32);// >= 1868: f_168

		f240 = buffer->Read<uint8_t>(1);
		if (f240)
		{
			f218 = buffer->Read<uint16_t>(16);

			if (f191)
			{
				f216 = buffer->Read<uint8_t>(8);
			}
		}

		buffer->Seek(0);

		auto object = g_pointers->m_get_net_object(*g_pointers->m_network_object_mgr, ownerNetId, true);
		auto entity = object ? object->GetGameObject() : nullptr;

		if (features::explosion_karma && gta_util::get_local_ped() && math::distance_between_vectors({ posX, posY, posZ }, *gta_util::get_local_ped()->m_navigation->get_position()) < 3.0f) {
			int id = player->m_player_id;
			g_fiber_pool->queue_job([id, explosionType, damageScale, cameraShake, isAudible, isInvisible] {
				Toxic::blame_explode_player(g_player_service->get_self(), g_player_service->get_by_id(id), explosionType, damageScale, isAudible, isInvisible, cameraShake);
				});
		}

		if (auto offset_object = g_pointers->m_get_net_object(*g_pointers->m_network_object_mgr, f210, true); f208 == 0 && entity && entity->m_entity_type == 4 && reinterpret_cast<CPed*>(entity)->m_player_info
			&& player->m_player_info->m_ped && player->m_player_info->m_ped->m_net_object
			&& ownerNetId != player->m_player_info->m_ped->m_net_object->m_object_id && !offset_object)
		{
			auto togType = protections::g_blockBlameExplosion.toggleType;
			if (togType == eToggleTypes::tog_none) {
				return;
			}
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Warning, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLAMED_FOR_EXPLOSION"), std::make_format_args(player->GetName(), reinterpret_cast<CPed*>(entity)->m_player_info->m_net_player_data.m_name));
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				return;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma) {
				MainNotification(ImGuiToastType_Warning, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLAMED_FOR_EXPLOSION"), std::make_format_args(player->GetName(), reinterpret_cast<CPed*>(entity)->m_player_info->m_net_player_data.m_name));
				g_fiber_pool->queue_job([entity, player] {
					Toxic::blame_explode_player(g_player_service->get_by_name(reinterpret_cast<CPed*>(entity)->m_player_info->m_net_player_data.m_name), g_player_service->get_by_id(player->m_player_id),
					EXP_TAG_BLIMP, (float)ENTITY::GET_ENTITY_MAX_HEALTH(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->m_player_id)), true, false, 0.f);
					});
			}
		}
	}

	void hooks::received_event(rage::netEventMgr* event_manager, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, int buffer_size, rage::datBitBuffer* buffer) {
		if (event_id > 91u) {
			g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}
		const auto* event_name = reinterpret_cast<char*>(event_manager) + 8 * event_id + 243376;
		if (event_name == nullptr || source_player == nullptr || source_player->m_player_id < 0 || source_player->m_player_id >= 32) {
			g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}
		auto plyr = g_player_service->get_by_id(source_player->m_player_id);
		if (plyr && plyr->block_all_script_events) {
			g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}
		switch (static_cast<eNetworkEvents>(event_id)) {
		case eNetworkEvents::KICK_VOTES_EVENT:
		{
			std::uint32_t player_bitfield = buffer->Read<uint32_t>(32);
			if (player_bitfield & (1 << target_player->m_player_id))
			{
				auto togType = protections::g_blockVoteKick.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Warning, 8000, TRANSLATE("WARNING"), TRANSLATE("VOTE_KICK_FROM_PLAYER"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Warning, 8000, TRANSLATE("WARNING"), TRANSLATE("VOTE_KICK_FROM_PLAYER"), source_player->GetName());
					auto playerService = g_player_service->get_by_id(source_player->m_player_id);
					g_fiber_pool->queue_job([=] {
						gta_util::get_network()->m_game_session.is_host() ? NETWORK::NETWORK_SESSION_KICK_PLAYER(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(source_player->m_player_id)) : Toxic::DesyncKick(source_player);
						});
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::NETWORK_INCREMENT_STAT_EVENT:
		{
			auto ise = std::make_unique<CNetworkIncrementStatEvent>();
			buffer->ReadDword(&ise->m_stat, 0x20);
			buffer->ReadDword(&ise->m_amount, 0x20);
			if (incrementStatEvent(ise.get(), source_player)) {
				g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPT_ENTITY_STATE_CHANGE_EVENT:
		{
			uint16_t entity = buffer->Read<uint16_t>(13);
			auto type = buffer->Read<ScriptEntityChangeType>(4);
			uint32_t unk = buffer->Read<uint32_t>(32);
			if (type == ScriptEntityChangeType::SettingOfTaskVehicleTempAction)
			{
				uint16_t ped_id = buffer->Read<uint16_t>(13);
				uint32_t action = buffer->Read<uint32_t>(8);

				if ((action >= 15 && action <= 18) || action == 33)
				{
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_VEH_TEMP_ACTION"), source_player->GetName());
					return;
				}
			}
			else if (type > ScriptEntityChangeType::SetVehicleExclusiveDriver || type < ScriptEntityChangeType::BlockingOfNonTemporaryEvents)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_INVALID_SCRIPT_ENTITY_CHANGE_TYPE"), source_player->GetName());
				g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPTED_GAME_EVENT:
		{
			const auto scripted_game_event = std::make_unique<CScriptedGameEvent>();
			buffer->ReadDword(&scripted_game_event->m_args_size, 32);
			if (scripted_game_event->m_args_size - 1 <= 0x1AF)
				buffer->ReadArray(&scripted_game_event->m_args, 8 * scripted_game_event->m_args_size);

			if (scripted_game_eventHk(scripted_game_event.get(), source_player)) {
				g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}
			buffer->Seek(0);

			break;
		}
		case eNetworkEvents::NETWORK_CLEAR_PED_TASKS_EVENT:
		{
			auto net_id = buffer->Read<int>(13);

			if (gta_util::get_local_ped() && gta_util::get_local_ped()->m_net_object && gta_util::get_local_ped()->m_net_object->m_object_id == net_id)
			{
				auto togType = protections::g_blockNetworkFreeze.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_FREEZE_FROM_PLAYER"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma) {
					MainNotification(ImGuiToastType_Warning, 8000, TRANSLATE("WARNING"), TRANSLATE("BLOCKED_FREEZE_FROM_PLAYER"), source_player->GetName());
					g_pointers->m_clearPedTasksNetwork(source_player->m_player_info->m_ped, true);
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::RAGDOLL_REQUEST_EVENT:
		{
			auto net_id = buffer->Read<int>(13);

			if (gta_util::get_local_ped() && gta_util::get_local_ped()->m_net_object && gta_util::get_local_ped()->m_net_object->m_object_id == net_id)
			{
				auto togType = protections::g_blockRagdollRequest.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_RAGDOLL_FROM_PLAYER"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma)
				{
					MainNotification(ImGuiToastType_Warning, 8000, TRANSLATE("WARNING"), TRANSLATE("BLOCKED_RAGDOLL_FROM_PLAYER"), source_player->GetName());
					if (auto ped = g_player_service->get_by_id(source_player->m_player_id)->get_ped()) {
						if (auto net_object = ped->m_net_object)
							g_pointers->m_request_ragdoll(net_object->m_object_id);
					}
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::MODIFY_PTFX_WORD_STATE_DATA_SCRIPTED_EVOLVE_EVENT:
		case eNetworkEvents::NETWORK_PTFX_EVENT:
		{
			auto togType = protections::g_blockNetworkPTFX.toggleType;
			if (togType == eToggleTypes::tog_none) {
				return;
			}
			if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_PTFX_FROM_PLAYER"), source_player->GetName());
			}
			if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
				g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}
			if (togType == eToggleTypes::tog_notifyBlockandKarma)
			{
				MainNotification(ImGuiToastType_Warning, 8000, TRANSLATE("WARNING"), TRANSLATE("BLOCKED_PTFX_FROM_PLAYER"), source_player->GetName());
				g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
				Redirect1945Moment(source_player);
				return;
			}
		}
		// Don't block this event, we still want to report this player
		// because if we still report others, our account seems less fishy
		case eNetworkEvents::REPORT_CASH_SPAWN_EVENT:
		{
			uint32_t money;

			buffer->Seek(64);
			buffer->ReadDword(&money, 32);
			buffer->Seek(0);

			if (money >= 2000)
			{
				MainNotification(ImGuiToastType_Warning, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("REPORT_CASH_SPAWN_EVENT"), source_player->GetName());
			}

			break;
		}
		// player sending this event is a modder
		case eNetworkEvents::REPORT_MYSELF_EVENT:
		{
			MainNotification(ImGuiToastType_Warning, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("REPORT_MYSELF_EVENT"), source_player->GetName());
			break;
		}
		case eNetworkEvents::REQUEST_CONTROL_EVENT:
		{
			auto net_id = buffer->Read<int>(13);
			if (gta_util::get_local_ped() && gta_util::get_local_ped()->m_vehicle
				&& gta_util::get_local_ped()->m_vehicle->m_net_object
				&& gta_util::get_local_ped()->m_vehicle->m_net_object->m_object_id == net_id) //The request is for a vehicle we are currently in.
			{
				Vehicle personal_vehicle = get_personal_vehicle();
				Vehicle veh = g_pointers->m_pointerToHandle(gta_util::get_local_ped()->m_vehicle);
				if (!NETWORK::NETWORK_IS_ACTIVITY_SESSION() || personal_vehicle == veh || DECORATOR::DECOR_GET_INT(veh, "RandomId") == gta_util::get_local_ped()->m_net_object->m_object_id) // Or it's a vehicle we spawned.
				{
					auto togType = protections::g_blockRagdollRequest.toggleType;
					if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_RQE_FROM_PLAYER"), source_player->GetName());
					}
					if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
						g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
						return;
					}
					if (togType == eToggleTypes::tog_notifyBlockandKarma) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_RQE_FROM_PLAYER"), source_player->GetName());
						g_fiber_pool->queue_job([=] {
							Toxic::blame_explode_player(g_player_service->get_by_id(source_player->m_player_id), g_player_service->get_by_id(source_player->m_player_id),
							EXP_TAG_BLIMP, 1000.f, false, false, 0.f);
							});
						g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
						return;
					}
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPT_WORLD_STATE_EVENT:
		{
			auto type = buffer->Read<WorldStateDataType>(4);
			buffer->Read<bool>(1);
			CGameScriptId id;
			script_id_deserialize(id, *buffer);

			if (type == WorldStateDataType::Rope)
			{
				buffer->Read<int>(9);    // network rope id
				buffer->Read<float>(19); // pos x
				buffer->Read<float>(19); // pos y
				buffer->Read<float>(19); // pos z
				buffer->Read<float>(19); // rot x
				buffer->Read<float>(19); // rot y
				buffer->Read<float>(19); // rot z
				buffer->Read<float>(16); // length
				int type = buffer->Read<int>(4);
				float initial_length = buffer->Read<float>(16);
				float min_length = buffer->Read<float>(16);

				if (type == 0 || initial_length < min_length) // https://docs.fivem.net/natives/?_0xE832D760399EB220
				{
					// most definitely a crash
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_ROPE_FROM_PLAYER"), source_player->GetName());
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}
			else if (type == WorldStateDataType::PopGroupOverride)
			{
				int pop_schedule = buffer->ReadSigned<int>(8); // Pop Schedule
				int pop_group = buffer->Read<int>(32);      // Pop Group
				int percentage = buffer->Read<int>(7);       // Percentage

				if (pop_group == 0 && (percentage == 0 || percentage == 103))
				{
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_PG0_FROM_PLAYER"), source_player->GetName());
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}
			else if (type > WorldStateDataType::VehiclePlayerLocking || type < WorldStateDataType::CarGen)
			{
				MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_IWST_FROM_PLAYER"), source_player->GetName());
				g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::ALTER_WANTED_LEVEL_EVENT:
		{
			uint16_t networkId{};
			buffer->ReadWord(&networkId, 13);
			if (auto netObj = g_pointers->m_get_net_object(*g_pointers->m_network_object_mgr, networkId, FALSE); netObj) {
				if (netObj->m_owner_id == gta_util::get_network_player_mgr()->m_local_net_player->m_player_id) {
					auto togType = protections::g_blockClearWanted.toggleType;
					if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_ALTER_WANTED_LVL_FROM_PLAYER"), source_player->GetName());
					}
					if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
						g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
						return;
					}
					if (togType == eToggleTypes::tog_notifyBlockandKarma) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_ALTER_WANTED_LVL_FROM_PLAYER"), source_player->GetName());
						g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
						return;
					}
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::REMOVE_WEAPON_EVENT:
		{
			int net_id = buffer->Read<int>(13);
			uint32_t hash = buffer->Read<uint32_t>(32);

			if (hash == "WEAPON_UNARMED"_joaat)
			{
				auto togType = protections::g_blockRemoveWeapon.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REMOVE_WEAPONS_FROM_PLAYER"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REMOVE_WEAPONS_FROM_PLAYER"), source_player->GetName());
					g_fiber_pool->queue_job([source_player] {
						for (const auto& weapons : weaponHashes)
							WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(source_player->m_player_id), weapons);
						});
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}

			if (gta_util::get_local_ped() && gta_util::get_local_ped()->m_net_object && gta_util::get_local_ped()->m_net_object->m_object_id == net_id)
			{
				auto togType = protections::g_blockRemoveWeapon.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REMOVE_WEAPON_ATTEMPT"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REMOVE_WEAPON_ATTEMPT"), source_player->GetName());
					g_fiber_pool->queue_job([source_player] {
						for (const auto& weapons : weaponHashes)
							WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(source_player->m_player_id), weapons);
						});
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::REMOVE_ALL_WEAPONS_EVENT:
		{
			int net_id = buffer->Read<int>(13);

			if (gta_util::get_local_ped() && gta_util::get_local_ped()->m_net_object && gta_util::get_local_ped()->m_net_object->m_object_id == net_id)
			{
				auto togType = protections::g_blockRemoveWeapons.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REMOVE_ALL_WEAPONS_ATTEMPT"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_REMOVE_ALL_WEAPONS_ATTEMPT"), source_player->GetName());
					g_fiber_pool->queue_job([source_player] {
						for (const auto& weapons : weaponHashes)
							WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(source_player->m_player_id), weapons);
						});
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::GIVE_WEAPON_EVENT: {
			int net_id = buffer->Read<int>(13);
			if (gta_util::get_local_ped() && gta_util::get_local_ped()->m_net_object && gta_util::get_local_ped()->m_net_object->m_object_id == net_id)
			{
				auto togType = protections::m_blockGiveWeapon.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_GIVE_WEAPONS_ATTEMPT"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_GIVE_WEAPONS_ATTEMPT"), source_player->GetName());
					g_fiber_pool->queue_job([source_player] {
						for (const auto& weapons : weaponHashes)
							WEAPON::GIVE_WEAPON_TO_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(source_player->m_player_id), weapons, 9999, 0, 1);
						});
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::GIVE_CONTROL_EVENT:
		{
			uint32_t timestamp = buffer->Read<uint32_t>(32);
			int count = buffer->Read<int>(2);
			bool all_objects_migrate_together = buffer->Read<bool>(1);
			if (count > 3)
			{
				count = 3;
			}
			for (int i = 0; i < count; i++)
			{
				int net_id = buffer->Read<int>(13);
				eNetObjType object_type = buffer->Read<eNetObjType>(4);
				int migration_type = buffer->Read<int>(3);

				if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
				{
					auto togType = protections::g_blockGiveControl.toggleType;
					if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("OUT_OF_BOUNDS_CONTROL_TYPE"), source_player->GetName());
					}
					if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
						g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
						return;
					}
					if (togType == eToggleTypes::tog_notifyBlockandKarma) {
						MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("OUT_OF_BOUNDS_CONTROL_TYPE"), source_player->GetName());
						g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
						return;
					}
				}
			}
			buffer->Seek(0);
			m_syncing_player = source_player;
			break;
		}
		case eNetworkEvents::NETWORK_PLAY_SOUND_EVENT:
		{
			bool is_entity = buffer->Read<bool>(1);
			std::int16_t entity_net_id;
			rage::fvector3 position;
			std::uint32_t ref_hash;
			if (is_entity)
				entity_net_id = buffer->Read<std::int16_t>(13);
			else {
				position.x = buffer->ReadSignedFloat(19, 1337.0f);
				position.y = buffer->ReadSignedFloat(19, 1337.0f);
				position.z = buffer->ReadFloat(19, 1337.0f);
			}
			bool has_ref = buffer->Read<bool>(1);
			if (has_ref)
				ref_hash = buffer->Read<std::uint32_t>(32);
			std::uint32_t sound_hash = buffer->Read<std::uint32_t>(32);
			if (sound_hash == "Remote_Ring"_joaat && plyr) {
				auto togType = protections::g_blockSoundSpam.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("SOUND_SPAM_FROM_PLAYER"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("SOUND_SPAM_FROM_PLAYER"), source_player->GetName());
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					auto soundPos = source_player->m_player_info->m_ped->GetNavigation()->get_position();
					AUDIO::PLAY_SOUND_FROM_COORD(-1, "Remote_Ring", Vector3(soundPos->x, soundPos->y, soundPos->z), "0", TRUE, 1000, 0);
					return;
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::EXPLOSION_EVENT:
		{
			if (plyr)
			{
				auto togType = protections::g_blockExplosion.toggleType;
				if (togType == eToggleTypes::tog_notify || togType == eToggleTypes::tog_notifyAndBlock) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_EXPLOSION"), source_player->GetName());
				}
				if (togType == eToggleTypes::tog_block || togType == eToggleTypes::tog_notifyAndBlock) {
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
				if (togType == eToggleTypes::tog_notifyBlockandKarma) {
					MainNotification(ImGuiToastType_Protections, 8000, TRANSLATE("PROTECTIONS"), TRANSLATE("BLOCKED_EXPLOSION"), source_player->GetName());
					g_fiber_pool->queue_job([source_player] {
						Toxic::blame_explode_player(g_player_service->get_by_id(source_player->m_player_id), g_player_service->get_by_id(source_player->m_player_id),
						EXP_TAG_BLIMP, (float)ENTITY::GET_ENTITY_MAX_HEALTH(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(source_player->m_player_id)), false, false, 0.f);
						});
					g_pointers->m_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return;
				}
			}

			scan_explosion_event(source_player, buffer);
			break;
		}
		case eNetworkEvents::WEAPON_DAMAGE_EVENT:
		{
			scan_weapon_damage_event(source_player, buffer);
			break;
		}
		default: break;
		}

		return g_hooking->m_receiveEventHk.getOg<decltype(&received_event)>()(event_manager, source_player, target_player, event_id, event_index, event_handled_bitset, buffer_size, buffer);
	}
}