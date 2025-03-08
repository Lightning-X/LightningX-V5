#include "api_service.hpp"
#include "../../HTTPRequest.hpp"
#include "../../Notify.hpp"
#include "../../Translations.hpp"
#include "../../Fibers/fiber_pool.hpp"
#include "../../Natives.hpp"
#include "../../Features.hpp"
namespace base
{
	api_service::api_service()
	{
		g_api_service = this;
	}

	api_service::~api_service()
	{
		g_api_service = nullptr;
	}

	bool api_service::isRidOnline(std::uint64_t rid) {
		http::Request request("http://scui.rockstargames.com/api/friend/getprofile");
		nlohmann::json body = { { "RockstarId", std::to_string(rid) } };
		if (auto scInfo = g_pointers->m_socialClubInfo; auto scTicket = scInfo->m_ticket) {
			auto response = request.send("POST", body.dump(), {
				{ "X-Requested-With", "XMLHttpRequest" },
				{ "Content-Type", "application/json; charset=utf-8" },
				{ "Authorization", "SCAUTH val=\"" + std::string(scTicket) + "\"" }
				});
			nlohmann::json parsedBody = nlohmann::json::parse(response.body.begin(), response.body.end());
			if (parsedBody["Status"].get<bool>()) {
				for (const auto& acc : parsedBody["Accounts"]) {
					const auto& accData = acc["RockstarAccount"];
					if (rid == accData["RockstarId"].get<int>()) {
						return accData["IsOnline"].get<bool>();
					}
				}
			}
			else {
				MainNotification(ImGuiToastType_Warning, 4200, TRANSLATE("NETWORK_RID_TOOLS"), TRANSLATE("ACCOUNT_WASNT_FOUND_ENTER_VALID_RID"));
			}
		}
		return false;
	}
	int api_service::nameToRid(const std::string& name) {
		http::Request request("http://scui.rockstargames.com/api/friend/accountsearch");
		nlohmann::json body = { { "searchNickname", name } };
		if (auto scInfo = g_pointers->m_socialClubInfo) {
			if (auto scTicket = scInfo->m_ticket) {
				auto response = request.send("POST", body.dump(), {
					{ "X-Requested-With", "XMLHttpRequest" },
					{ "Content-Type", "application/json; charset=utf-8" },
					{ "Authorization", "SCAUTH val=\"" + std::string(scTicket) + "\"" }
					});
				auto parsedBody = nlohmann::json::parse(response.body.begin(), response.body.end());
				if (parsedBody["Status"].get<bool>() && parsedBody["Total"].get<int>() > 0) {
					return parsedBody["Accounts"][0]["RockstarId"].get<int>();
				}
			}
		}
		MainNotification(ImGuiToastType_Warning, 4200, TRANSLATE("NETWORK_RID_TOOLS"), TRANSLATE("ACCOUNT_WASNT_FOUND_ENTER_VALID_RID"));
		return 0;
	}
	std::string api_service::ridToName(uint64_t rid) {
		http::Request request("http://scui.rockstargames.com/api/friend/getprofile");
		nlohmann::json body = { {"RockstarId", std::to_string(rid)} };
		auto scInfo = g_pointers->m_socialClubInfo;
		if (!scInfo) {
			return "null";
		}
		auto scTicket = scInfo->m_ticket;
		if (!scTicket) {
			return "null";
		}
		auto response = request.send("POST", body.dump(), {
			{"X-Requested-With", "XMLHttpRequest"},
			{"Content-Type", "application/json; charset=utf-8"},
			{"Authorization", "SCAUTH val=\"" + std::string(scTicket) + "\""}
			});
		if (auto parsedBody = nlohmann::json::parse(response.body.begin(), response.body.end()); parsedBody["Status"].get<bool>()) {
			for (auto& acc : parsedBody["Accounts"]) {
				if (rid == acc["RockstarAccount"]["RockstarId"].get<int>()) {
					return acc["RockstarAccount"]["Name"].get<std::string>();
				}
			}
			MainNotification(ImGuiToastType_Warning, 4200, TRANSLATE("NETWORK_RID_TOOLS"), TRANSLATE("ACCOUNT_WASNT_FOUND_ENTER_VALID_RID"));
		}
		return "null";
	}

	// Ratelimit: 10 per Minute, if exceeded than 5 min cooldown
	bool api_service::send_socialclub_message(uint64_t rid, std::string_view message)
	{
		http::Request request("https://scui.rockstargames.com/api/messaging/sendmessage");
		nlohmann::json body = { {std::format(R"({{"env":"prod","title":"gta5","version":11,"recipientRockstarId":"{}","messageText":"{}"}})", rid, message)} };
		if (auto scInfo = g_pointers->m_socialClubInfo; auto scTicket = scInfo->m_ticket) {
			auto response = request.send("POST", body.dump(), {
				{ "X-Requested-With", "XMLHttpRequest" },
				{ "Content-Type", "application/json; charset=utf-8" },
				{ "Authorization", "SCAUTH val=\"" + std::string(scTicket) + "\"" }
				});
			auto parsedBody = nlohmann::json::parse(response.body.begin(), response.body.end());
			return response.status.Ok;
		}
	}
	/*void api_service::ridJoinViaInvite(std::uint64_t rid) {
		g_fiber_pool->queue_job([this, rid] {
			auto plyrHndle = rage::rlGamerHandle(rid);
			auto success = false;
			rage::rlTaskStatus state{};
			if (rage::rlSessionByGamerTaskResult taskRes; g_pointers->m_getSessionByGamerHandle(0, &plyrHndle, 1, &taskRes, 1, &success, &state)) {
				while (state.status == 1) script::get_current()->yield();
				if (state.status == 3 && success) {
					rage::rlPresenceEventInviteAccepted inv(gta_util::get_local_playerinfo()->m_net_player_data, taskRes.m_session_info, taskRes.m_gamer_handle);
					g_pointers->m_sendInviteAcceptedPresenceEvent(g_pointers->m_presecneStruct, &inv, 0x21);
					return;
				}
			}
			if (isRidOnline(rid)) MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_RID_TOOLS"), TRANSLATE("FAILED_TO_JOIN_UNK_REASON"));
			else MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_RID_TOOLS"), TRANSLATE("FAILED_TO_JOIN_DUE_PLAYER_BIENG_OFFLINE"));
			return;
			});
	}*/
	void api_service::join_session(const rage::rlSessionInfo& info) {
		if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS()) {
			MainNotification(ImGuiToastType_Warning, 4200, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("PLAYER_SWITCH_IN_PROGRESS"));
			return;
		}
		g_api_service->isQueued = true;
		g_api_service->info = info;
		features::sessionJoin({ eSessionType::NEW_PUBLIC });
		script::get_current()->yield(350ms);
		const uint32_t running_threads = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat);
		if (running_threads == 0) {
			g_api_service->isQueued = false;
			MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_RID_JOINER"), TRANSLATE("UNABLE_TO_LAUNCH_MAIN_TRANSITION"));
		}
	}

	void api_service::ridJoinViaSession(uint64_t rid) {
		base::g_fiber_pool->queue_job([this, rid] {
			if (const uint32_t running_threads = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat); running_threads != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS()) {
				MainNotification(ImGuiToastType_Warning, 4200, TRANSLATE("NETWORK_RID_JOINER"), TRANSLATE("PLAYER_SWITCH_IN_PROGRESS"));
				return;
			}
			rage::rlGamerHandle player_handle(rid);
			rage::rlSessionByGamerTaskResult result{};
			bool success = false;
			if (rage::rlTaskStatus state{}; g_pointers->m_getSessionByGamerHandle(0, &player_handle, 1, &result, 1, &success, &state)) {
				while (state.status == 1) {
					script::get_current()->yield();
				}
				if (state.status == 3 && success) {
					join_session(result.m_session_info);
					return;
				}
			}
			if (isRidOnline(rid)) {
				MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_RID_TOOLS"), TRANSLATE("FAILED_TO_JOIN_UNK_REASON"));
			}
			else {
				MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_RID_TOOLS"), TRANSLATE("FAILED_TO_JOIN_DUE_PLAYER_BIENG_OFFLINE"));
			}
			return;
			});
	}

	/*void api_service::ridJoinCFR(uint64_t rid) {
		g_fiber_pool->queue_job([=]() {
			if (isRidOnline(rid)) {
				static auto friendRegistry = g_pointers->m_friendRegistry;
				static auto playerName = ridToName(rid);
				auto fiberName = std::format("ridjoinFbr_{}_rid{}", playerName, rid);
				g_tmpRid = rid;
				g_tmpFbrName = fiberName;

				if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS()) {
					MainNotification(ImGuiToastType_Error, 4200, TRANSLATE("NETWORK_RID_JOINER"), TRANSLATE("UNABLE_TO_LAUNCH_MAIN_TRANSITION"));
					return;
				}

				HUD::ACTIVATE_FRONTEND_MENU(0xD528C7E2, true, 2);
				HUD::ACTIVATE_FRONTEND_MENU(0xD528C7E2, true, 2);

				auto friendOnIndexZero = friendRegistry->get(0);
				friendRegistry->m_friend_count = 1;
				friendOnIndexZero->m_friend_state = 3;
				friendOnIndexZero->m_is_joinable = 1;
				while (true) {
					friendRegistry->m_friend_count = 1;
					friendOnIndexZero->m_friend_state = 3;
					friendOnIndexZero->m_is_joinable = 1;
					friendOnIndexZero->m_rockstar_id = g_tmpRid;
					memcpy(friendOnIndexZero->m_name, TRANSLATE("JOIN"), sizeof(friendOnIndexZero->m_name));
					if (friendOnIndexZero->m_is_joinable) {
						auto message_hash = HUD::GET_WARNING_SCREEN_MESSAGE_HASH();
						if (message_hash == 15890625 || message_hash == -587688989)
							PAD::SET_CONTROL_VALUE_NEXT_FRAME(2, 201, 1.0);
					}
					if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) != 0) {
						break;
					}
					script::get_current()->yield();
				}
			}
			else
			{
				MainNotification(ImGuiToastType_Error, 5000, "RID JOINER", "PLAYER IS OFFLINE!");
				return;
			}
			});
	}*/
}