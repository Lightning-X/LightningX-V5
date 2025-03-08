#include "Hooking.hpp"
#include "NetGameEvent.hpp"
#include "GtaUtill.hpp"
#include "Natives.hpp"
#include "Services/Player/player_service.hpp"
#include "GTAV-Classes-master/network/Network.hpp"
#include "GTAV-Classes-master/network/netTime.hpp"
#include "Translations.hpp"
#include "KicksAndCrashes.hpp"
#include "Features.hpp"
#include "Notify.hpp"
#include "Services/Api/api_service.hpp"
static std::vector<const char*> spam_texts = {
		"qq", //a chinese chat app
		"QQ",
		"WWW.",
		"www.",
		".TOP",
		".COM",
		".top",
		"\xE3\x80\x90", //left bracket in Chinese input method
		"/Menu",
		"Money/",
		"Money\\\\",
		// "\xD0\xBC\xD0\xB5", // Cyrillic "me"
		"P888",
		"gtacash",
		"\xE6\x89\xA3\xE6\x89\xA3", // no clue what this is
		"\xE5\xBE\xAE\xE4\xBF\xA1", // "wechat" in Chinese
		".cc",
		"<font s",
		"sellix.io",
		"ezcars",
		"PLANO INICIAL", // "initial plan"
		"REP +",
		"20R$", // Brazil currency?
		"l55.me",
		"\xE5\xBA\x97", //"shop" in Chinese
		"\xE9\x92\xB1", //"money" in Chinese
		"\xE5\x88\xB7", //"make(money)" in Chinese
		// disabled as it's too verbose
		// "av", //uknowwhat video
		"\xE8\x90\x9D\xE8\x8E\x89", //"cute girl" in Chinese
		"\xE5\xA6\x88",             //"mother" in Chinese
		"\xE7\xBE\x8E\xE5\xA5\xB3", //"sexy girl" in Chinese
		"\xE5\xBC\xBA\xE5\xA5\xB8", //"rape" in Chinese
		"\xE8\x90\x9D",             //"loli" in Chinese
		"\xE6\x8C\x82",             //"hack" in Chinese
		"\xE5\x85\x83",             //chinese dollar
		"TRUSTPILOT",
		"cashlounge",
		"Fast Delivery",
};

namespace base::spam
{
	inline bool is_text_spam(const char* text)
	{
		for (auto e : spam_texts)
			if (strstr(text, e) != 0)
				return true;

		return false;
	}

	inline void log_chat(char* msg, player_ptr player, bool is_spam) {
		auto const& plData = *player->get_net_data();
		auto ip = player->get_ip_address();
		g_logger->send(Darkmagenta, TRANSLATE("CHAT"), std::format("From: {} | RID: {} | IP: {}.{}.{}.{} | {}", player->get_name(), plData.m_gamer_handle.m_rockstar_id, (int)ip.m_field1, (int)ip.m_field2, (int)ip.m_field3, (int)ip.m_field4, msg));
	}
}

inline void gamer_handle_deserialize(rage::rlGamerHandle& hnd, rage::datBitBuffer& buf)
{
	constexpr int PC_PLATFORM = 3;
	if ((hnd.m_platform = buf.Read<uint8_t>(8)) != PC_PLATFORM)
		return;

	buf.ReadInt64((int64_t*)&hnd.m_rockstar_id, 64);
	hnd.unk_0009 = buf.Read<uint8_t>(8);
}

inline bool is_kick_instruction(rage::datBitBuffer& buffer)
{
	rage::rlGamerHandle sender, receiver;
	char name[18];
	gamer_handle_deserialize(sender, buffer);
	gamer_handle_deserialize(receiver, buffer);
	buffer.ReadString(name, 17);
	int instruction = buffer.Read<int>(32);
	return instruction == 8;
}

namespace base
{
	bool get_msg_type(rage::eNetMessage& msgType, rage::datBitBuffer& buffer)
	{
		uint32_t pos;
		uint32_t magic;
		uint32_t length;
		uint32_t extended{};
		if ((buffer.m_flagBits & 2) != 0 || (buffer.m_flagBits & 1) == 0 ? (pos = buffer.m_curBit) : (pos = buffer.m_maxBit),
			buffer.m_bitsRead + 15 > pos || !buffer.ReadDword(&magic, 14) || magic != 0x3246 || !buffer.ReadDword(&extended, 1))
		{
			msgType = rage::eNetMessage::MsgInvalid;
			return false;
		}
		length = extended ? 16 : 8;
		if ((buffer.m_flagBits & 1) == 0 ? (pos = buffer.m_curBit) : (pos = buffer.m_maxBit),
			length + buffer.m_bitsRead <= pos && buffer.ReadDword((uint32_t*)&msgType, length))
			return true;
		else
			return false;
	}

	static void script_id_deserialize(CGameScriptId& id, rage::datBitBuffer& buffer)
	{
		id.m_hash = buffer.Read<uint32_t>(32);
		id.m_timestamp = buffer.Read<uint32_t>(32);

		if (buffer.Read<bool>(1))
			id.m_position_hash = buffer.Read<uint32_t>(32);

		if (buffer.Read<bool>(1))
			id.m_instance_id = buffer.Read<int32_t>(8);
	}

	bool hooks::receive_net_message(void* netConnectionManager, void* a2, rage::netConnection::InFrame* frame)
	{
		if (frame->get_event_type() != rage::netConnection::InFrame::EventType::FrameReceived)
			return g_hooking->m_receiveNetMessageHk.getOg<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);

		if (frame->m_data == nullptr || frame->m_length == 0)
			return g_hooking->m_receiveNetMessageHk.getOg<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);

		rage::datBitBuffer buffer(frame->m_data, frame->m_length);
		buffer.m_flagBits = 1;

		rage::eNetMessage msgType;
		player_ptr player;

		for (std::uint32_t i = 0; i < gta_util::get_network()->m_game_session_ptr->m_player_count; i++)
		{
			if (gta_util::get_network()->m_game_session_ptr->m_players[i]->m_player_data.m_peer_id_2 == frame->m_peer_id)
			{
				player = g_player_service->get_by_host_token(
					gta_util::get_network()->m_game_session_ptr->m_players[i]->m_player_data.m_host_token);
				break;
			}
		}

		if (!get_msg_type(msgType, buffer))
			return g_hooking->m_receiveNetMessageHk.getOg<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);

		if (player)
		{
			switch (msgType)
			{
			case rage::eNetMessage::MsgTextMessage:
			case rage::eNetMessage::MsgTextMessage2:
			{
				char message[256];
				buffer.ReadString(message, 256);

				if (player->is_spammer)
					return true;

				if (spam::is_text_spam(message))
				{
					if (features::log_chat_messages)
						spam::log_chat(message, player, true);
					player->is_spammer = true;
					if (features::kick_advertisers) {
						if (g_player_service->get_self()->is_host())
							Toxic::HostKick(player);
						Toxic::NullFunctionKick(player);
						Toxic::ScriptHostKick(player);
						Toxic::BailKick(player);
						Toxic::EndSessionKick(player);
						Toxic::DesyncKick(player->get_net_game_player());
					}
					return true;
				}
				else
				{
					if (features::log_chat_messages)
						spam::log_chat(message, player, false);
				}
				break;
			}
			case rage::eNetMessage::MsgScriptMigrateHost:
			{
				std::chrono::system_clock::time_point m_last_transition_msg_sent{};
				int m_num_failed_transition_attempts = 0;
				if (std::chrono::system_clock::now() - m_last_transition_msg_sent < 200ms) {
					if (m_num_failed_transition_attempts++ == 20) {
						MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("PLAYER_TRIED_TO_OOM_KICK_YOU"), player->get_name());
						player->is_modder = true;
					}
					return true;
				}
				else {
					m_last_transition_msg_sent = std::chrono::system_clock::now();
					m_num_failed_transition_attempts = 0;
				}
				break;
			}
			case rage::eNetMessage::MsgRemoveGamersFromSessionCmd:
			{
				player_ptr pl;
				uint64_t session_id;
				buffer.ReadQWord(&session_id, 64);
				uint32_t count;
				buffer.ReadDword(&count, 6);
				for (std::uint32_t i = 0; i < count; i++)
				{
					uint64_t peer_id;
					buffer.ReadQWord(&peer_id, 64);
					for (std::uint32_t i = 0; i < gta_util::get_network()->m_game_session_ptr->m_peer_count; i++)
					{
						if (gta_util::get_network()->m_game_session_ptr->m_peers[i]->m_peer_data.m_peer_id_2 == peer_id)
						{
							pl = g_player_service->get_by_host_token(
								gta_util::get_network()->m_game_session_ptr->m_peers[i]->m_peer_data.m_host_token);
							break;
						}
					}
				}

				if (player && pl && player->id() != pl->id() && count == 1 && frame->m_msg_id == -1)
				{
					if (g_player_service->get_self()->is_host())
					{
						MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("BREAKUP_KICK_DETECTED"), player->get_name(), pl->get_name());
						return true;
					}
					else
					{
						MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("BREAKUP_KICK_DETECTED"), player->get_name(), pl->get_name());
					}
				}
				break;
			}
			case rage::eNetMessage::MsgNetComplaint:
			{
				uint64_t host_token{};
				buffer.ReadQWord(&host_token, 64);

				std::vector<CNetGamePlayer*> players;

				uint32_t num_of_tokens{};
				buffer.ReadDword(&num_of_tokens, 32);

				if (player && host_token != player->get_net_data()->m_host_token)
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("DETECTED_DESYNC_PROTECTION"), player->get_name());
				}
				return true; // block desync kicks as host
			}
			case rage::eNetMessage::MsgScriptHostRequest:
			{
				CGameScriptId script;
				script_id_deserialize(script, buffer);
				if (script.m_hash == "freemode"_joaat && features::force_script_host)
					return true;
				break;
			}
			case rage::eNetMessage::MsgNetTimeSync:
			{
				int action = buffer.Read<int>(2);
				uint32_t counter = buffer.Read<uint32_t>(32);
				uint32_t token = buffer.Read<uint32_t>(32);
				uint32_t timestamp = buffer.Read<uint32_t>(32);
				uint32_t time_diff = (*g_pointers->m_network_time)->m_time_offset + frame->m_timestamp;

				if (action == 0)
				{
					player->player_time_value = timestamp;
					player->player_time_value_received_time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
					if (!player->time_difference || time_diff > player->time_difference.value())
						player->time_difference = time_diff;
				}
				break;
			}
			case rage::eNetMessage::MsgTransitionGamerInstruction:
			{
				// it doesn't work but a certain p2c uses it
				if (is_kick_instruction(buffer)) {
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("DETECTED_INSTRUCTIONAL_KICK"), player->get_name());
					return true;
				}
				break;
			}
			case rage::eNetMessage::MsgKickPlayer:
			{
				uint64_t sessionId;
				uint64_t peerId;
				buffer.ReadQWord(&sessionId, 0x40);
				buffer.ReadQWord(&peerId, 0x40);
				rage::eKickReason reason = buffer.Read<rage::eKickReason>(0x3);
				buffer.Read<int>(0x20);
				switch (reason) {
				case rage::eKickReason::VotedOut: {
					MainNotification(ImGuiToastType_Protections, 10000, TRANSLATE("PROTECTIONS"), TRANSLATE("YOU_GOT_KICKED_BY_HOST"));
				} break;
				case rage::eKickReason::PeerComplaints: {
					MainNotification(ImGuiToastType_Protections, 10000, TRANSLATE("PROTECTIONS"), TRANSLATE("YOU_GOT_KICKED_BY_PEER"), player->get_name());
					break;
				}
				case rage::eKickReason::ConnectionError: {
					MainNotification(ImGuiToastType_Protections, 10000, TRANSLATE("PROTECTIONS"), TRANSLATE("YOU_GOT_KICKED_CONNECTION_ERROR"));
					break;
				}
				}
			}
			}
		}
		return g_hooking->m_receiveNetMessageHk.getOg<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);
	}
}